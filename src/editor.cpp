#include "editor.hpp"
#include "track.hpp"

#include "viewport/viewport.hpp"
#include "viewport/indicator.hpp"

#include "gui/welcome_screen.hpp"
#include "gui/msg_wndw.hpp"
#include "gui/new_dialog_wndw.hpp"
#include "gui/toolbar.hpp"
#include "gui/toolbox.hpp"
#include "gui/env_panel.hpp"
#include "gui/terr_panel.hpp"
#include "gui/road_panel.hpp"
#include "gui/extra_panel.hpp"
#include "gui/tex_sel.hpp"
#include "gui/road_cross_section_wndw.hpp"

#include "mesh/driveline.hpp"
#include "mesh/road.hpp"
#include "mesh/terrain.hpp"

#include <physfs.h>
#include <iostream>
#include <fstream>
#include <assert.h>

#if _IRR_MATERIAL_MAX_TEXTURES_ < 6
    #error Configure your irr for more textures... check readme
#endif

#define MAX_VALID_STRING_SIZE 200

Editor* Editor::m_editor = nullptr;

// ----------------------------------------------------------------------------
bool Editor::buttonClicked(s32 ID)
{
    RoadPanel* rp;
    switch (ID)
    {
    // ToolBar buttons
    case ToolBar::TBI_UNDO:
        m_viewport->undo();
        return true;
    case ToolBar::TBI_REDO:
        m_viewport->redo();
        return true;
    case ToolBar::TBI_SELECT:
        m_viewport->setState(Viewport::SELECT);
        return true;
    case ToolBar::TBI_MOVE:
        m_viewport->setEditMode(Viewport::MOVE);
        return true;
    case ToolBar::TBI_ROTATE:
        m_viewport->setEditMode(Viewport::ROTATE);
        return true;
    case ToolBar::TBI_SCALE:
        m_viewport->setEditMode(Viewport::SCALE);
        return true;
    case ToolBar::TBI_DELETE:
        m_viewport->deleteCmd();
        return true;
    case ToolBar::TBI_CAM:
        m_viewport->setState(Viewport::FREECAM);
        return true;
    case ToolBar::TBI_RECAM:
        m_viewport->restoreCam();
        return true;
    case ToolBar::TBI_DRIVELINE:
        m_viewport->selectDriveLine();
        return true;
    case ToolBar::TBI_HIDE_TERRAIN:
        m_viewport->getTerrain()->swapVisibility();
        return true;
    case ToolBar::TBI_TRY:
        runTrack();
        return true;
    case ToolBar::TBI_MUSIC:
        m_viewport->lock();
        m_gui_env->addFileOpenDialog(L"Select music:", true, 0, 1234, true, m_music_loc);
        return true;
        // WELCOME SCREEN
    case ToolBar::TBI_SAVE:
        if (m_viewport->getTrack())
            m_viewport->getTrack()->save();
        return true;
    case ToolBar::TBI_EXPORT:
        m_viewport->build();
        return true;
    // ToolBox BTN:
    case ToolBox::TWND_ID:
    case ToolBox::EWND_ID:
    case ToolBox::RWND_ID:
    case ToolBox::XWND_ID:
        m_toolbox->setWndw(ID);
        return true;
    // ToolBox / Terrain buttons:
    case TerrPanel::EDGE_1:
    case TerrPanel::EDGE_2:
    case TerrPanel::EDGE_3:
    case TerrPanel::M_T1:
    case TerrPanel::M_T2:
    case TerrPanel::M_T3:
    case TerrPanel::M_T4:
    case TerrPanel::H_BTN:
    case TerrPanel::T_SOFT_BTN:
    case TerrPanel::T_HARD_BTN:
    case TerrPanel::T_BRIGHTNESS_BTN:
         m_viewport->setState(Viewport::TERRAIN_MOD);
    case TerrPanel::M_TC1:
    case TerrPanel::M_TC2:
    case TerrPanel::M_TC3:
    case TerrPanel::M_TC4:
    case TerrPanel::S_T1:
    case TerrPanel::S_T2:
    case TerrPanel::S_T3:
    case TerrPanel::S_T4:
    case TerrPanel::S_T5:
    case TerrPanel::S_T6:
        TerrPanel::getTerrPanel()->btnDown(ID);
         return true;
    // ToolBox / RoadPanel buttons:
    case RoadPanel::CREATE:
        rp = RoadPanel::getRoadPanel();
        Viewport::get()->getTrack()->createRoad(rp->getNextRoadType(),
                                                rp->getNextRoadName());
        rp->btnDown(ID);
        return true;
    case RoadPanel::ADD:
    case RoadPanel::INSERT:
        m_viewport->setSplineMode(true);
        m_viewport->setState(Viewport::SPLINE);
        RoadPanel::getRoadPanel()->btnDown(ID);
        return true;
    case RoadPanel::CHECKLINE:
        m_viewport->setState(Viewport::CHECK_LINE);
        return true;
    case RoadPanel::EXIT:
        m_viewport->setState(Viewport::SELECT);
        return true;
    case RoadPanel::CROSS_SECTION:
        m_viewport->switchRoadCrossSectionMode(false);
        return true;
    case RoadPanel::ATTACH_TO_DL:
        m_viewport->attachRoadToDriveLine();
        return true;
    case RoadPanel::TEXTURE:
        for (int i = RoadPanel::getRoadPanel()->getSelectedRoadID(); i != 0; i = 0)
        {
            m_tex_sel->subscribe((Road*)m_viewport->getTrack()->getRoadByID(i));
        }
        return true;
    // ToolBox / Extra buttons:
    case ExtraPanel::BTN_BANANA:
    case ExtraPanel::BTN_ITEM:
    case ExtraPanel::BTN_SNITRO:
    case ExtraPanel::BTN_BNITRO:
        addItem(ID);
        return true;
    default:
        break;
    }

    EnvPanel* ep = EnvPanel::getEnvPanel();
    // env panel
    if (ID >= ep->FIRST_BTN_ID &&
        ID < ep->FIRST_BTN_ID + ep->getBtnNum())
    {
        // element is picked from env panel
        m_viewport->setSplineMode(false);
        m_viewport->setNewEntity(EnvPanel::getEnvPanel()->getModelPathFromBtnId(ID));
        return true;
    }
    if (ID == ep->FIRST_BTN_ID + ep->getBtnNum())
    {
        ep->switchPage(-1);
        return true;
    }
    if (ID == ep->FIRST_BTN_ID + ep->getBtnNum() + 1)
    {
        ep->switchPage(1);
        return true;
    }

    if (ID >= m_tex_sel->FIRST_BTN_ID &&
        ID < m_tex_sel->FIRST_TEX_BTN_ID + (s32)m_tex_sel->getBtnNum())
    {
        m_tex_sel->btnClicked(ID);
        return true;
    }

    std::cerr << "Button click isn't handled!" << std::endl;
    return false;
} // buttonClicked

// ----------------------------------------------------------------------------
bool Editor::importantButtonClicked(int ID)
{
    stringc s;
    switch (ID)
    {
        // ToolBar buttons
    case ToolBar::TBI_EXIT:
        m_msg_wndw->showMsg(_(L"Do you really want to quit?\n"
                            L"Any unsaved progress will be lost!"), true, true);
        return true;
    case ToolBar::TBI_NEW:
        if (m_viewport) m_viewport->lock();
        m_new_dialog_wndw->show();
        return true;
    case WelcomeScreen::FBTN_ID + 1:
        m_welcome_screen->hide();
    case ToolBar::TBI_OPEN:
        m_new_dialog_wndw->hide();
        m_gui_env->addFileOpenDialog(L"Open track:", true, 0, -1, true, m_maps_path);
        if (m_viewport) m_viewport->lock();
        return true;
        // WELCOME SCREEN
    case WelcomeScreen::FBTN_ID:
        s = m_welcome_screen->getActiveText();
        if (s != "")
        {
            if (open(path(m_maps_path) + "/" + m_welcome_screen->getActiveText()))
                m_welcome_screen->hide();
        }
        return true;
    case WelcomeScreen::FBTN_ID + 2:
        m_welcome_screen->hide();
        m_new_dialog_wndw->show();
        return true;
    case NewDialogWndw::BTN_ID:
        newTrack();
    case NewDialogWndw::BTN_ID+1:
        m_new_dialog_wndw->hide();
        return true;
    case MsgWndw::OK_BTN_ID:
        m_msg_wndw->buttonClicked();
        return true;
    case MsgWndw::CA_BTN_ID:
        m_msg_wndw->buttonClicked(true);
        return true;
    case RoadCrossSectionWndw::OK:
        m_viewport->switchRoadCrossSectionMode(true);
        return true;
    case RoadCrossSectionWndw::CANCEL:
        m_viewport->switchRoadCrossSectionMode(false);
        return true;
    case RoadCrossSectionWndw::POINT_M:
    case RoadCrossSectionWndw::POINT_P:
        m_viewport->clearSelection();
    case RoadCrossSectionWndw::GRID_ON_OFF:
    case RoadCrossSectionWndw::GRID_M:
    case RoadCrossSectionWndw::GRID_P:
    case RoadCrossSectionWndw::SYM_ON_OFF:
        m_rcs->buttonClicked(ID);
        return true;
    }
    return false;
} // importantButtonClicked

// ----------------------------------------------------------------------------
void Editor::simpleShortcuts(EKEY_CODE code)
{
    switch (code)
    {
    case KEY_ESCAPE:
        if (m_viewport->escPressed())
            m_msg_wndw->showMsg(_(L"Do you really want to quit?\n"
                                  L"Any unsaved progress will be lost!"), true, true);
        return;
    case KEY_DELETE:
        m_viewport->deleteCmd();
        return;
    case KEY_KEY_R:
        m_viewport->selectDriveLine();
        return;
    case KEY_KEY_C:
        m_viewport->setState(Viewport::FREECAM);
        return;
    case KEY_KEY_T:
        m_viewport->getTerrain()->swapVisibility();
        return;
    default:
        return;
    }
} // simpleShortcuts

// ----------------------------------------------------------------------------
void Editor::shiftShortcuts(EKEY_CODE code)
{
    switch (code)
    {
    case KEY_KEY_A:
        m_viewport->setState(Viewport::SELECT);
        break;
    case KEY_KEY_S:
        m_viewport->scale();
        break;
    case KEY_KEY_G:
        m_viewport->move();
        break;
    case KEY_KEY_R:
        m_viewport->setEditMode(Viewport::ROTATE);
        m_viewport->rotate();
        break;
    default:
        break;
    }
} // ctrlShortcuts

// ----------------------------------------------------------------------------
void Editor::ctrlShortcuts(EKEY_CODE code)
{
    switch (code)
    {
    case KEY_KEY_S:
        if (m_viewport->getTrack())
            m_viewport->getTrack()->save();
        break;
    case KEY_KEY_Z:
        m_viewport->undo();
        break;
    case KEY_KEY_Y:
        m_viewport->redo();
        break;
    case KEY_KEY_N:
        m_new_dialog_wndw->show();
        break;
    case KEY_KEY_O:
        m_new_dialog_wndw->hide();
        m_gui_env->addFileOpenDialog(L"Open track:", true, 0, -1, true, m_maps_path);
        break;
    default:
        break;
    }
} // ctrlShortcuts

// ----------------------------------------------------------------------------
bool Editor::isValidSize(u8 size)
{
    if (size < 1 || size > MAX_VALID_STRING_SIZE)
    {
        std::cerr << "File loading failed:";
        std::cerr << " File contains invalid string size.\n";
        return false;
    }
    return true;
} // isValidSize

// ----------------------------------------------------------------------------
bool Editor::init()
{
    m_maps_path         = NULL;
    m_music_loc         = NULL;
    m_indicator         = NULL;
    m_valid_data_dir    = false;
    m_exe_loc           = "";
    m_toolbar           = NULL;
    m_toolbox           = NULL;
    m_rcs               = NULL;
    m_tex_sel           = NULL;
    m_indicator         = NULL;
    m_viewport          = NULL;
    m_rcs               = NULL;
    m_font              = NULL;
    m_new_dialog_wndw   = NULL;

    m_screen_size = dimension2du(10, 10);

    IrrlichtDevice *nulldevice = createDevice(video::EDT_NULL);
    m_screen_size = nulldevice->getVideoModeList()->getDesktopResolution();
    readConfigFile(nulldevice->getFileSystem());
    nulldevice->drop();

    m_device = createDevice(EDT_OPENGL, m_screen_size, 16, false, false, true);
    if (!m_device) return false;

    m_device->setResizable(true);
    m_device->setWindowCaption(L"SuperTuxKart Track Editor Beta v0.03");

    if (m_screen_size.Width < 20 || m_screen_size.Height < 20)
        m_device->maximizeWindow();

    m_video_driver  = m_device->getVideoDriver();
    m_scene_manager = m_device->getSceneManager();
    m_gui_env       = m_device->getGUIEnvironment();

    m_screen_size   = m_video_driver->getScreenSize();
    m_def_wd        = m_device->getFileSystem()->getWorkingDirectory();

    // lights
    m_scene_manager->setAmbientLight(SColorf(0.3f, 0.3f, 0.3f, 1.0f));
    ILightSceneNode* l = m_scene_manager->addLightSceneNode(0, vector3df(0, 1, 0),
                                              SColorf(1.0f, 1.0f, 1.0f), 500, -1);
    l->setLightType(ELT_DIRECTIONAL);
    l->setPosition(vector3df(0,
        1, 0));

    m_device->setEventReceiver(this);

    if (!isValidDataLoc()) dataDirLocDlg();

    return true;
} // init

// ----------------------------------------------------------------------------
void Editor::initAfterDataDirKnown()
{
    m_rcs = RoadCrossSectionWndw::get();

    // fonts
    IGUISkin* skin = m_gui_env->getSkin();
    m_font = m_gui_env->getFont(m_data_loc + L"editor/font/font.xml");
    skin->setFont(m_font);

    // removing gui transparency
    for (s32 i = 0; i < EGDC_COUNT; ++i)
    {
        video::SColor col = skin->getColor((EGUI_DEFAULT_COLOR)i);
        col.setAlpha(255);
        skin->setColor((EGUI_DEFAULT_COLOR)i, col);
    }

    // free camera
    ICameraSceneNode* cam;
    cam = m_scene_manager->addCameraSceneNodeMaya();
    cam->setID(1);
    cam->setFarValue(20000.f);
    cam->setTarget(vector3df(0, 0, 0));
    cam->setInputReceiverEnabled(false);

    // viewport init
    ICameraSceneNode* norm_cam;
    norm_cam = m_scene_manager->addCameraSceneNode(0, vector3df(25, 50, 30),
        vector3df(5, 10, 6));
    norm_cam->setID(2);
    m_viewport = Viewport::get(norm_cam, &m_mouse, &m_keys);
    m_viewport->setFreeCamera(cam);
    m_indicator = m_viewport->getIndicator();
    m_scene_manager->setActiveCamera(norm_cam);

    m_msg_wndw = MsgWndw::get();

    m_toolbar = ToolBar::getToolBar();
    m_new_dialog_wndw = NewDialogWndw::get();
    m_new_dialog_wndw->hide();
    m_welcome_screen = WelcomeScreen::get();
} // initAfterDataDirKnown


// ----------------------------------------------------------------------------
void Editor::readConfigFile(IFileSystem* file_system)
{
    IXMLReader* xml_reader = file_system->createXMLReader(path(PHYSFS_getBaseDir())
                                                                    + "config.xml");

    if (!xml_reader)
    {
        path dir = PHYSFS_getUserDir();
        m_config_loc = dir + "/.stk-te";
        xml_reader = file_system->createXMLReader(m_config_loc + "/config.xml");
        if (!xml_reader)
        {
            PHYSFS_setWriteDir(dir.c_str());
            PHYSFS_mkdir(".stk-te");
            return;
        }
    }
    else m_config_loc = PHYSFS_getBaseDir();

    const stringw node_name(L"data_dir");
    const stringw res(L"res");
    const stringw exe(L"exe");
    while (xml_reader->read())
    {
        if (xml_reader->getNodeType() == EXN_ELEMENT)
        {
            if (res.equals_ignore_case(xml_reader->getNodeName()))
            {
                m_screen_size = dimension2du(
                    atol(((stringc)xml_reader->getAttributeValueSafe(L"x")).c_str()),
                    atol(((stringc)xml_reader->getAttributeValueSafe(L"y")).c_str()));
            }
            else  if (node_name.equals_ignore_case(xml_reader->getNodeName()))
            {
                m_data_loc = xml_reader->getAttributeValueSafe(L"path");
                m_icons_loc = m_data_loc + "editor/icons/";
            }
            else if (exe.equals_ignore_case(xml_reader->getNodeName()))
            {
                m_exe_loc = xml_reader->getAttributeValueSafe(L"path");
            }
        }
    }
    xml_reader->drop();
} // readConfigFile

// ----------------------------------------------------------------------------
void Editor::writeResAndExePathIntoConfig()
{
    stringc p;
    IFileSystem* file_system = m_device->getFileSystem();
    IXMLReader*  xml_reader = file_system->createXMLReader(m_config_loc + "/config.xml");
    if (xml_reader)
    {
        const stringw node_name(L"data_dir");
        while (xml_reader->read())
        {
            if (xml_reader->getNodeType() == EXN_ELEMENT
                && node_name.equals_ignore_case(xml_reader->getNodeName()))
            {
                p = xml_reader->getAttributeValueSafe(L"path");
            }
        }
        xml_reader->drop();
    }

    std::ofstream f;
    f.open((m_config_loc + "/config.xml").c_str());
    f << "<config>\n";
    f << "  <data_dir path=\"" << p.c_str() << "\" />\n";

    if (!m_exe_loc.empty())
        f << "  <exe path=\"" << m_exe_loc.c_str() << "\" />\n";

    f << "  <res x=\"" << m_screen_size.Width << "\" y=\"";
    f << m_screen_size.Height << "\" />\n";
    f << "</config>\n";
    f.close();

} // writeResAndExePathIntoConfig

// ----------------------------------------------------------------------------
bool Editor::isValidDataLoc()
{
    IFileSystem* file_system = m_device->getFileSystem();
    if (file_system->addFileArchive((m_data_loc + "textures").c_str(),
        false, false, EFAT_FOLDER, "", &m_tex_dir))
    {
        initDataLoc();
        return true;
    }
    return false;
} // validDataLoc

// ----------------------------------------------------------------------------
bool Editor::validateDataLoc(path file)
{
    IFileSystem* file_system = m_device->getFileSystem();
    if (!file_system->addFileArchive((file + "textures").c_str(), false,
                                            false, EFAT_FOLDER, "", &m_tex_dir))
        return false;
    m_data_loc = file;
    m_icons_loc = m_data_loc + "editor/icons/";

    initDataLoc();

    std::ofstream f;
    f.open((m_config_loc + "/config.xml").c_str());
    f << "<config>\n";
    f << "  <data_dir path=\"" << file.c_str() << "\" />\n";
    f << "</config>\n";
    f.close();

    return true;
} // validDataLoc

// ----------------------------------------------------------------------------
void Editor::initDataLoc()
{
    IFileSystem* file_system = m_device->getFileSystem();
    if (!file_system->addFileArchive((m_data_loc + "editor/xml").c_str(),
        false, false, EFAT_FOLDER, "", &m_xml_dir))
    {
        std::cerr << "Bad news: i couldn't find the xml directory.\n";
        std::cerr << "Maybe the whole editor folder is missing? :(\n";
        exit(-1);
    }
    file_system->addFileArchive(m_data_loc, false, false, EFAT_FOLDER);
    m_valid_data_dir = true;

    initAfterDataDirKnown();

    path p = m_data_loc.c_str();
    p += "editor/maps";
    m_maps_path = new c8[p.size() + 1];
    strcpy(m_maps_path, p.c_str());

    if (!PHYSFS_exists(p.c_str()))
    {
        std::cerr << "Warning: maps directory doesn't exist. Creating...\n";
        PHYSFS_setWriteDir(m_data_loc.c_str());
        PHYSFS_mkdir("editor/maps");
    }

    p = m_data_loc.c_str();
    p += "music";
    m_music_loc = new c8[p.size() + 1];
    strcpy(m_music_loc, p.c_str());

    m_track_dir = m_data_loc.c_str();
    m_track_dir += "tracks/";

    file_system->changeWorkingDirectoryTo(m_def_wd);

    m_tex_sel = TexSel::getTexSel();
    m_toolbox = ToolBox::getToolBox();

    file_system->addFileArchive((m_data_loc + "editor").c_str(),
        false, false, EFAT_FOLDER);

} // setDataLoc

// ----------------------------------------------------------------------------
void Editor::dataDirLocDlg()
{
    IGUIFileOpenDialog* ofd = m_gui_env->addFileOpenDialog(
        L"Pls. select data directory (folder containing textures, tracks, etc.):",
        true, 0, -1, true);
    ofd->setMinSize(dimension2du(600, 512));
    ofd->setRelativePosition(position2di(m_screen_size.Width / 2 - 300, 100));
} // dataDirLocDlg

// ----------------------------------------------------------------------------
void Editor::runTrack()
{
    if (m_exe_loc.empty())
    {
        m_gui_env->addFileOpenDialog(L"Select STK binary:", true, 0, 333,true);
        if (m_viewport) m_viewport->lock();
        return;
    }
    stringc msg = "\"";
    msg += m_exe_loc + "\" --no-start-screen --track=";
    msg += m_viewport->getTrack()->getFileName();
    system(msg.c_str());
} // runTrack

// ----------------------------------------------------------------------------
Editor* Editor::getEditor(dimension2du screen_size)
{
    if (m_editor != nullptr) return m_editor;

    m_editor = new Editor();
    m_editor->m_screen_size = screen_size;

    if (!m_editor->init()) return nullptr;
    return m_editor;
} // getEditorf

// ----------------------------------------------------------------------------
void Editor::render()
{
    m_video_driver->beginScene(true, true, SColor(255, 120, 80, 170));

    if (m_indicator)
        m_indicator->renderToTexture();
    m_scene_manager->drawAll();

    if (m_viewport)
        m_viewport->draw();
    if (m_rcs)
        m_rcs->render();

    m_gui_env->drawAll();

    if (m_viewport && m_indicator && m_viewport->getState() != Viewport::FREECAM
                                                          && !m_rcs->isVisible())
        m_indicator->drawToScreen();

    m_video_driver->endScene();
} // render


// ----------------------------------------------------------------------------
bool Editor::run()
{
    if (!m_device) return 0;

    long current_time = m_device->getTimer()->getTime();
    long last_time    = current_time;

    while (m_device->run())
    {
        if (m_device->isWindowActive())
        {
            current_time = m_device->getTimer()->getTime();
            if (m_valid_data_dir && !m_msg_wndw->isVisible())
                m_viewport->animate(current_time - last_time);

            // drawing
            //m_video_driver->beginScene(true, true, SColor(255, 80, 0, 170));
            render();

            if (m_video_driver->getScreenSize() != m_screen_size)
            {
                m_screen_size = m_video_driver->getScreenSize();
                if (m_toolbar) m_toolbar->reallocate();
                if (m_toolbox) m_toolbox->reallocate();
                if (m_tex_sel) m_tex_sel->reallocate();
                if (m_rcs)     m_rcs->reallocate();
                if (m_indicator) m_indicator->reallocate();
                if (m_new_dialog_wndw) m_new_dialog_wndw->reallocate(m_screen_size);
            }
            last_time = current_time;
        } // isWindowActive
        else m_device->yield();
    } // while device->run

    if (m_config_loc!="")
    {
        writeResAndExePathIntoConfig();
    }

    if (m_maps_path)
        delete[] m_maps_path;
    if (m_music_loc)
        delete[] m_music_loc;
    if (m_indicator)
        delete m_indicator;

    delete m_viewport;
    delete m_toolbox;
    delete m_toolbar;

    m_device->drop();

    return 1;
} // run

// ----------------------------------------------------------------------------
bool Editor::OnEvent(const SEvent& event)
{
    if (event.EventType == EET_GUI_EVENT
       && event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
            if (importantButtonClicked(event.GUIEvent.Caller->getID()))
                return true;

    if (!m_valid_data_dir)
    {
        if (event.EventType == EET_GUI_EVENT)
        {
            if (event.GUIEvent.EventType == EGET_DIRECTORY_SELECTED)
            {
                if (validateDataLoc(
                    ((IGUIFileOpenDialog*)event.GUIEvent.Caller)->getDirectoryName()))
                {
                    ((IGUIFileOpenDialog*)event.GUIEvent.Caller)->remove();
                }
                return true;
            } // Directory selected
            if (event.GUIEvent.EventType == EGET_FILE_CHOOSE_DIALOG_CANCELLED)
            {
                dataDirLocDlg();
                return true;
            }
        } // GUI_EVENT
        return false;
    } // !valid data dir

    if (!m_viewport) return false;

    // mouse event
    if (event.EventType == EET_MOUSE_INPUT_EVENT)
    {
        // check if mouse is outside of the viewport's domain
        if (event.MouseInput.Y < 50 || event.MouseInput.X <= 15 ||
            event.MouseInput.X >(s32) m_screen_size.Width - 250 ||
             event.MouseInput.Y >= (s32)m_screen_size.Height - 15 ||
            (event.MouseInput.X >(s32)m_screen_size.Width - 500 && m_tex_sel->isActive()))
        {
            m_toolbox->setScrollBarsEnabled(true);
            u32 s = m_viewport->getState();
            if (s == Viewport::EDIT ||
               (s==Viewport::SELECT && m_mouse.middle_btn_down))
                    keepMouseIn(event.MouseInput.X, event.MouseInput.Y);
            else
            {
                m_viewport->setFocus(false);
                m_mouse.in_view = false;
                return false;
            }
        } // mouse is outside
        else // mouse is inside
        {
            m_toolbox->setScrollBarsEnabled(false);
        }
        m_mouse.in_view = true;
        m_viewport->setFocus(true);
        m_mouse.refresh(event);
    } // mouse event

    if (event.EventType == EET_GUI_EVENT
        && event.GUIEvent.EventType == EGET_FILE_SELECTED)
    {
        switch (((IGUIFileOpenDialog*)event.GUIEvent.Caller)->getID())
        {
        case 1234:
            m_viewport->getTrack()->setMusic(toRelative(
                path(((IGUIFileOpenDialog*)event.GUIEvent.Caller)->getFileName())));
            break;
        case 333:
            m_exe_loc = path(((IGUIFileOpenDialog*)event.GUIEvent.Caller)->getFileName());
            writeResAndExePathIntoConfig();
            runTrack();
            break;
        default:
            open(path(((IGUIFileOpenDialog*)event.GUIEvent.Caller)->getFileName()));
        }
        m_viewport->lock(false);
        return true;
    }

    if (!m_viewport->getTrack()) return false;

    if (event.EventType == EET_GUI_EVENT)
    {
        RoadPanel* rp;
        s32 id = event.GUIEvent.Caller->getID();
        switch (event.GUIEvent.EventType)
        {
        case EGET_FILE_CHOOSE_DIALOG_CANCELLED:
            m_viewport->lock(false);
            return true;
        case EGET_BUTTON_CLICKED:
            m_gui_env->removeFocus(m_gui_env->getFocus()->getParent());
            return buttonClicked(id);

        case EGET_COMBO_BOX_CHANGED:
            switch (id)
            {
            case EnvPanel::CB_ID:
                EnvPanel::getEnvPanel()->resetIndex();
                EnvPanel::getEnvPanel()->refreshBtnTable();
                return true;
            case RoadPanel::SELECT:
                m_viewport->setActiveRoad(RoadPanel::getRoadPanel()
                                                     ->getSelectedRoadID());
                break;
            default:
                break;
            }
            return false;

        case EGET_EDITBOX_CHANGED:
            switch (id)
            {
            case TerrPanel::H_MAX_EDIT_BOX:
            case TerrPanel::H_MIN_EDIT_BOX:
                TerrPanel::getTerrPanel()->refreshTerrModData();
                return true;
            case EnvPanel::SF_ID:
                EnvPanel::getEnvPanel()->resetIndex();
                EnvPanel::getEnvPanel()->refreshBtnTable();
                return true;
            case TexSel::SEARCH_BOX:
                TexSel::getTexSel()->searchFieldDirty();
                return true;
            default:
                break;
            }
            return false;

        case EGET_SCROLL_BAR_CHANGED:
            switch (id)
            {
            case TerrPanel::V_SCROLL:
                TerrPanel::getTerrPanel()->updatePanelComponentPositions();
                return true;
            case TerrPanel::INTENSITY:
            case TerrPanel::RADIUS:
                TerrPanel::getTerrPanel()->refreshTerrModData();
                m_viewport->setState(Viewport::TERRAIN_MOD);
                return true;
            case RoadPanel::V_SCROLL:
                RoadPanel::getRoadPanel()->updatePanelComponentPositions();
                return true;
            case RoadPanel::DETAIL:
                rp = RoadPanel::getRoadPanel();
                m_viewport->getTrack()->getRoadByID(rp->getSelectedRoadID())
                                                ->setDetail(rp->getDetail());
                return true;
            case RoadPanel::WIDTH:
                rp = RoadPanel::getRoadPanel();
                m_viewport->getTrack()->getRoadByID(rp->getSelectedRoadID())
                                                 ->setWidth(rp->getWidth());
                return true;
            case RoadPanel::WRAP_U:
                rp = RoadPanel::getRoadPanel();
                m_viewport->getTrack()->getRoadByID(rp->getSelectedRoadID())
                                                ->setUWrapCount(rp->getTexUCount());
                return true;
            case RoadPanel::WRAP_V:
                rp = RoadPanel::getRoadPanel();
                m_viewport->getTrack()->getRoadByID(rp->getSelectedRoadID())
                    ->setVWrapCount(rp->getTexVCount());
                return true;
            case RoadPanel::OFFSET_U:
                rp = RoadPanel::getRoadPanel();
                m_viewport->getTrack()->getRoadByID(rp->getSelectedRoadID())
                    ->setUOffset(rp->getTexUOffset());
                return true;
            case RoadPanel::OFFSET_V:
                rp = RoadPanel::getRoadPanel();
                m_viewport->getTrack()->getRoadByID(rp->getSelectedRoadID())
                    ->setVOffset(rp->getTexVOffset());
                return true;
            default:
                break;
            }
            return false;

        case EGET_CHECKBOX_CHANGED:
            switch (id)
            {
            case TerrPanel::H_MAX_CHECK_BOX:
            case TerrPanel::H_MIN_CHECK_BOX:
                TerrPanel::getTerrPanel()->refreshTerrModData();
                m_viewport->setState(Viewport::TERRAIN_MOD);
                return true;
            case RoadPanel::CLOSED_ROAD:
                m_viewport->roadClosedStateChanged();
                return true;
            case RoadPanel::SWAP_UV:
                m_viewport->roadSwapStateChanged();
                return true;
            default:
                break;
            }

        default:
            return false;
        }
    } // EventType == EET_GUI_EVENT

    bool should_quit = false;
    // gui active, there is nothing we should do
    if (m_viewport->isLocked()) should_quit = true;

    if (m_gui_env->getFocus())
    {
        s32 id = m_gui_env->getFocus()->getID();
        if (id == RoadPanel::NAMEBOX || id == EnvPanel::SF_ID
            || id == TerrPanel::H_MAX_EDIT_BOX || id == TerrPanel::H_MIN_EDIT_BOX)
            should_quit = true;
    }

    // keyboard event
    if (event.EventType == EET_KEY_INPUT_EVENT)
    {
        if (!event.KeyInput.PressedDown || !should_quit)
            m_keys.keyEvent(event.KeyInput.Key, event.KeyInput.PressedDown);

        if (should_quit) return false;

        if (event.KeyInput.PressedDown)
        {
            if (m_keys.state(SHIFT_PRESSED) && m_keys.state(CTRL_PRESSED))
            {
                if (m_keys.state(S_PRESSED))     m_viewport->build();
            }
            else
            {
                if (m_keys.state(SHIFT_PRESSED)) shiftShortcuts(event.KeyInput.Key);
                if (m_keys.state(CTRL_PRESSED))  ctrlShortcuts(event.KeyInput.Key);
                if (!m_keys.state(CTRL_PRESSED) && !m_keys.state(SHIFT_PRESSED))
                    simpleShortcuts(event.KeyInput.Key);
            }
        }
            return true;
    }

    return false;

} // OnEvent

// ----------------------------------------------------------------------------
void Editor::keepMouseIn(s32 sx, s32 sy)
{
    s32 x = sx;
    s32 y = sy;
    if (sy < 50)
        y = m_screen_size.Height - sy;
    else if (sy >= (s32)m_screen_size.Height - 15)
        y = 55;
    if (sx <= 15)
        x = (s32)m_screen_size.Width - 250;
    else if (sx >= (s32)m_screen_size.Width - 250)
        x = 20;
    m_device->getCursorControl()->setPosition(x, y);
    m_mouse.prev_x = x;
    m_mouse.prev_y = y;
} // keepMouseIn

// ----------------------------------------------------------------------------
bool Editor::open(path p)
{
    closeTrack();
    m_device->getFileSystem()->changeWorkingDirectoryTo(m_def_wd);
    if (m_viewport->setTrack(new Track(p.c_str())))
    {
        TerrPanel* tp = TerrPanel::getTerrPanel();
        tp->refreshTerrainTextures(m_viewport->getTerrain()->getMaterial(0));
        m_viewport->setSplineMode(false);
        m_viewport->setState(Viewport::SELECT);
        RoadPanel::getRoadPanel()->updateRoadList();
        return true;
    }
    else return false;
} // open

// ----------------------------------------------------------------------------
void Editor::newTrack()
{
    closeTrack();
    f32 size = (f32)m_new_dialog_wndw->getSize();
    Track* t = new Track(size, size);
    t->setFileName(m_new_dialog_wndw->getFileName());
    t->setTrackName(m_new_dialog_wndw->getTrackName());
    t->setDesigner(m_new_dialog_wndw->getDesigner());
    t->setGravityRoad(m_new_dialog_wndw->gravRoad());
    m_viewport->setTrack(t);
    m_viewport->setSplineMode(false);

    RoadPanel::getRoadPanel()->updateRoadList();

} // newTrack

// ----------------------------------------------------------------------------
void Editor::closeTrack()
{
    m_viewport->clear();
} // closeTrack

// ----------------------------------------------------------------------------
void Editor::addToRecentlyOpenedList(stringc name)
{
    std::list<stringc> list;
    list = readRecentlyOpenedList();

    std::list<stringc>::iterator it;
    for (it = list.begin(); it != list.end(); it++)
        if ((*it) == name) return;

    if (list.size() >= EVIL_NUMBER)
        list.pop_back();
    list.push_front(name);


    std::ofstream f;
    f.open((m_config_loc + "/recent.xml").c_str());

    f << "<files>\n";

    it = list.begin();
    for (; it != list.end(); it++)
    {
        f << "  <file name=\"" << (*it).c_str() << "\" />\n";
    }

    f << "</files>\n";

    f.close();
} // addToRecentlyOpenedList

// ----------------------------------------------------------------------------
std::list<stringc> Editor::readRecentlyOpenedList()
{
    std::list<stringc> list;

    IFileSystem* file_system = m_device->getFileSystem();
    IXMLReader*  xml_reader = file_system->createXMLReader(m_config_loc + "/recent.xml");

    stringc s;
    if (xml_reader)
    {
        const stringw file(L"file");
        while (xml_reader->read())
        {
            if (xml_reader->getNodeType() == EXN_ELEMENT
                && file.equals_ignore_case(xml_reader->getNodeName()))
            {
                s  = xml_reader->getAttributeValueSafe(L"name");
                list.push_back(s);
            }
        }
        xml_reader->drop();
    }
    return list;
} // readRecentlyOpenedList

// ----------------------------------------------------------------------------
void Editor::addItem(u32 id)
{
    ISceneNode* node = 0;
    switch (id)
    {
    case ExtraPanel::BTN_BANANA:
        node = m_scene_manager->addAnimatedMeshSceneNode(
            m_scene_manager->getMesh("models/banana.b3d"));
        node->setName("banana");
        break;
    case ExtraPanel::BTN_ITEM:
        node = m_scene_manager->addAnimatedMeshSceneNode(
            m_scene_manager->getMesh("models/gift-box.b3d"));
        node->setName("item");
        break;
    case ExtraPanel::BTN_SNITRO:
        node = m_scene_manager->addAnimatedMeshSceneNode(
            m_scene_manager->getMesh("models/nitrotank-small.b3d"));
        node->setName("small-nitro");
        break;
    case ExtraPanel::BTN_BNITRO:
        node = m_scene_manager->addAnimatedMeshSceneNode(
            m_scene_manager->getMesh("models/nitrotank-big.b3d"));
        node->setName("big-nitro");
        break;
    default:
        break;
    }
    m_viewport->setNewEntity(node);
} // addItem

// ----------------------------------------------------------------------------
path Editor::toRelative(path p)
{
    stringc path = p.c_str();
    s32 ix = path.findLast('/');
    if (ix == -1) return p;
    stringc ret = path.subString(ix+1, path.size() - ix);
    return ret;

} // toRelative

// ----------------------------------------------------------------------------
ITexture* Editor::loadImg(const stringw& file_path)
{
    return m_editor->m_video_driver->getTexture(file_path);
} // loadImg

// ----------------------------------------------------------------------------
stringc Editor::getTexStr(ITexture* tex)
{
    stringc h;
    u32 ix;
    h = tex->getName();
    ix = h.findLast('/');
    h = h.subString(ix + 1, h.size() - ix - 1);
    return h;
} // getTexStr

// ----------------------------------------------------------------------------
void Editor::writeStrc(FILE* fp, stringc str)
{
    u8 size;
    size = str.size() + 1;
    fwrite(&size, sizeof(u8), 1, fp);
    fwrite(str.c_str(), sizeof(c8), size, fp);
} // writeStrc

// ----------------------------------------------------------------------------
void Editor::readTexSt(FILE* fp, ITexture** tex)
{
    u8 size;
    fread(&size, sizeof(u8), 1, fp);

    if (!isValidSize(size))
    {
        *tex = 0;
        return;
    }
    c8* cc = new c8[size];
    fread(cc, sizeof(c8), size, fp);
    path p = cc;
    *tex = Editor::loadImg(p);
    delete[] cc;
} // readTexSt

// ----------------------------------------------------------------------------
stringc Editor::getLib(stringc s)
{
    u32 ix;
    ix = s.findLast('/');
    s = s.subString(0, ix);
    ix = s.findLast('/');
    s = s.subString(ix + 1, s.size() - ix - 1);
    return s;
} // getLib
