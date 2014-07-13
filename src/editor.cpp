#include "editor.hpp"
#include "track.hpp"

#include "viewport/viewport.hpp"
#include "viewport/indicator.hpp"

#include "gui/new_dialog_wndw.hpp"
#include "gui/toolbar.hpp"
#include "gui/toolbox.hpp"
#include "gui/env_panel.hpp"
#include "gui/terr_panel.hpp"
#include "gui/road_panel.hpp"
#include "gui/extra_panel.hpp"
#include "gui/tex_sel.hpp"

#include "mesh/driveline.hpp"
#include "mesh/iroad.hpp"

#include <physfs.h>
#include <iostream>
#include <fstream>
#include <assert.h>

Editor* Editor::m_editor = 0;

// ----------------------------------------------------------------------------
bool Editor::buttonClicked(int ID)
{
    char* c = 0;
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
        m_viewport->setState(Viewport::MOVE);
        return true;
    case ToolBar::TBI_ROTATE:
        m_viewport->setState(Viewport::ROTATE);
        return true;
    case ToolBar::TBI_SCALE:
        m_viewport->setState(Viewport::SCALE);
        return true;
    case ToolBar::TBI_DELETE:
        m_viewport->deleteCmd();
        return true;
    case ToolBar::TBI_CAM:
        m_viewport->setState(Viewport::FREECAM);
        return true;
    case ToolBar::TBI_SPLINE:
        m_viewport->setSplineMode(!m_viewport->getSplineMode());
        return true;
    case ToolBar::TBI_TRY:
        m_viewport->build();
        return true;
    case ToolBar::TBI_SAVE:
        if (m_viewport->getTrack())
            m_viewport->getTrack()->save();
        return true;
    case ToolBar::TBI_SAVE_AS:
        m_viewport->setState(Viewport::CHECK_LINE);
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
    case RoadPanel::DL_CREATE:
        rp = RoadPanel::getRoadPanel();
        Viewport::get()->getTrack()->createRoad(rp->getNextRoadType(),
                                                rp->getNextRoadName());
        rp->btnDown(ID);
        return true;
    case RoadPanel::DL_ADD:
    case RoadPanel::DL_INSERT:
        m_viewport->setSplineMode(true);
        m_viewport->setState(Viewport::SPLINE);
        RoadPanel::getRoadPanel()->btnDown(ID);
        return true;
    case RoadPanel::DL_EXIT:
        m_viewport->setState(Viewport::SELECT);
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
        m_viewport->setNewEntity(EnvPanel::getEnvPanel()->getModelPathFromBtnId(ID),
                                 EnvPanel::getEnvPanel()->getModelNameFromBtnId(ID));
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
        ID < m_tex_sel->FIRST_BTN_ID + m_tex_sel->getBtnNum() + 2)
    {
        m_tex_sel->btnClicked(ID);
        return true;
    }

    std::cerr << "Button click isn't handled!" << std::endl;
    return false;
} // buttonClicked

bool Editor::importantButtonClicked(int ID)
{
    switch (ID)
    {
        // ToolBar buttons
    case ToolBar::TBI_EXIT:
        m_device->closeDevice();
        return true;
    case ToolBar::TBI_NEW:
        m_new_dialog_wndw->show();
        return true;
    case ToolBar::TBI_OPEN:
        m_new_dialog_wndw->hide();
        m_gui_env->addFileOpenDialog(L"Open track:", true, 0, -1, false, m_maps_path);
        return true;
        // new window
    case NewDialogWndw::BTN_ID:
        newTrack();
        m_new_dialog_wndw->hide();
        return true;
    }
    return false;
} // importantButtonClicked

// ----------------------------------------------------------------------------
bool Editor::init()
{
    m_maps_path = 0;
    m_valid_data_dir = false;
	m_device = createDevice(EDT_OPENGL, m_screen_size, 16, false, false, true);
	if (!m_device) return false;

	m_device->setResizable(true);
	m_device->setWindowCaption(L"SuperTuxKart Track Editor");

	m_video_driver  = m_device->getVideoDriver();
	m_scene_manager = m_device->getSceneManager();
	m_gui_env       = m_device->getGUIEnvironment();

    m_def_wd        = m_device->getFileSystem()->getWorkingDirectory();

    // fonts
    IGUISkin* skin = m_gui_env->getSkin();
    IGUIFont* font = m_gui_env->getFont(L"font/font2.png");
    skin->setFont(font);

    // removing gui transparency
    for (s32 i = 0; i<EGDC_COUNT; ++i)
    {
        video::SColor col = skin->getColor((EGUI_DEFAULT_COLOR)i);
        col.setAlpha(255);
        skin->setColor((EGUI_DEFAULT_COLOR)i, col);
    }

    // lights
    m_scene_manager->setAmbientLight(SColorf(0.3f, 0.3f, 0.3f, 1.0f));
    ILightSceneNode* l = m_scene_manager->addLightSceneNode(0, vector3df(0, 1, 0),
                                              SColorf(1.0f, 1.0f, 1.0f), 500, -1);
    l->setLightType(ELT_DIRECTIONAL);
    l->setPosition(vector3df(0, 1, 0));

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
        vector3df(25, -30, -15));
    norm_cam->setID(2);
    m_viewport = Viewport::get(norm_cam, &m_mouse, &m_keys);
    m_viewport->setFreeCamera(cam);
    m_indicator = m_viewport->getIndicator();
    m_scene_manager->setActiveCamera(norm_cam);


    // track init - temporary!!!

    // Track* t = new Track(50, 50);
    // m_viewport->setTrack(t);
    // track init


    m_toolbar = ToolBar::getToolBar();
    m_new_dialog_wndw = NewDialogWndw::get();

    fileInit();

    m_device->setEventReceiver(this);
    return true;
} // init


// ----------------------------------------------------------------------------
void Editor::fileInit()
{    
    IFileSystem* file_system = m_device->getFileSystem();

    file_system->addFileArchive(PHYSFS_getBaseDir(), false, false, EFAT_FOLDER);
    IXMLReader* xml_reader = file_system->createXMLReader("config.xml");
    if (!xml_reader)
    {
        path dir = PHYSFS_getUserDir();
        m_config_loc = dir + "/.stk-te";

        if (!file_system->addFileArchive(m_config_loc.c_str(), false, false, EFAT_FOLDER))
        {
            PHYSFS_setWriteDir(dir.c_str());
            PHYSFS_mkdir(".stk-te");
            file_system->addFileArchive(m_config_loc, false, false, EFAT_FOLDER);
        }
        
        xml_reader = file_system->createXMLReader("config.xml");

        if (!xml_reader)
        {
            std::ofstream f;
            f.open("config.xml");
            f.close();
            xml_reader = file_system->createXMLReader("config.xml");
            assert(xml_reader);
        }
    }
    else m_config_loc = PHYSFS_getBaseDir();
    if (!validDataLoc(xml_reader)) dataDirLocDlg();
    xml_reader->drop();
} // fileInit

// ----------------------------------------------------------------------------
bool Editor::validDataLoc(IXMLReader* xml)
{
    IFileSystem* file_system = m_device->getFileSystem();
    path data_dir = "";
    const stringw element(L"data_dir");
    while (xml->read())
    {
        if (xml->getNodeType() == EXN_ELEMENT)
        {
            if (element.equals_ignore_case(xml->getNodeName()))
            {
                data_dir = xml->getAttributeValueSafe(L"path");
            }
        }
    }
    
    if (file_system->addFileArchive((data_dir + "textures").c_str(), 
        false, false, EFAT_FOLDER, "", &m_tex_dir))
    {
        m_valid_data_dir = true;

        path p = data_dir + "editor/maps";
        m_maps_path = new c8[p.size()+1];
        strcpy(m_maps_path, p.c_str());

        m_track_dir = data_dir + "tracks/";

        file_system->addFileArchive((data_dir + "editor/env/xml").c_str(),
            false, false, EFAT_FOLDER, "", &m_xml_dir);

        file_system->addFileArchive(data_dir, false, false, EFAT_FOLDER);
        
        m_tex_sel = TexSel::getTexSel();
        m_toolbox = ToolBox::getToolBox();

        file_system->addFileArchive((data_dir + "editor").c_str(),
            false, false, EFAT_FOLDER);

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


    file_system->addFileArchive((file + "editor/env/xml").c_str(),
        false, false, EFAT_FOLDER, "", &m_xml_dir);

    std::ofstream f;

    f.open((m_config_loc + "/config.xml").c_str());

    f << "<config>\n";
    f << "  <data_dir path=\"" << file.c_str() << "\" />\n";
    f << "</config>\n";

    path p = file.c_str();
    p += "editor";
    m_maps_path = new c8[p.size()+1];
    strcpy(m_maps_path, p.c_str());

    m_track_dir = file.c_str();
    m_track_dir+= "tracks/";

    f.close();
    
    return true;
} // validDataLoc

// ----------------------------------------------------------------------------
void Editor::dataDirLocDlg()
{
    IGUIFileOpenDialog* ofd = m_gui_env->addFileOpenDialog(
        L"Pls. select data directory (folder containing textures, tracks, etc.):",
        true, 0, -1, false);
    ofd->setMinSize(dimension2du(600, 512));
    ofd->setRelativePosition(position2di(m_screen_size.Width / 2.0 - 300, 100));
} // dataDirLocDlg


// ----------------------------------------------------------------------------
Editor* Editor::getEditor(dimension2du screen_size)
{
	if (m_editor != 0) return m_editor;

	m_editor = new Editor();
    m_editor->m_screen_size = screen_size;
	if (!m_editor->init()) return 0;
	return m_editor;
} // getEditorf

// ----------------------------------------------------------------------------
bool Editor::run()
{
	if (!m_device) return 0;

    long current_time = m_device->getTimer()->getTime();
    long last_time    = current_time;

	while (m_device->run())
    {
        current_time = m_device->getTimer()->getTime();
        if (m_valid_data_dir)
            m_viewport->animate(current_time - last_time);

		// drawing
		m_video_driver->beginScene(true, true, SColor(255, 80, 0, 170));

        
        m_indicator->renderToTexture();

        m_scene_manager->drawAll();
		m_gui_env->drawAll();
        m_viewport->draw();
        
        if (m_viewport->getState() != Viewport::FREECAM)
            m_indicator->drawToScreen();

		m_video_driver->endScene();

        if (m_video_driver->getScreenSize() != m_screen_size)
        {
            m_screen_size = m_video_driver->getScreenSize();
            m_toolbar->reallocate();
            m_toolbox->reallocate();
        }
        last_time = current_time;
    }

    if (m_maps_path)
        delete m_maps_path;

    delete m_viewport;
    delete m_toolbox;
    delete m_toolbar;

    // crash ???
    // m_device->drop();

	return 1;
} // run

// ----------------------------------------------------------------------------
bool Editor::OnEvent(const SEvent& event)
{

    if (event.EventType == EET_GUI_EVENT
        && event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
            importantButtonClicked(event.GUIEvent.Caller->getID());

    if (!m_valid_data_dir)
    {
        if (event.EventType == EET_GUI_EVENT)
        {
            if (event.GUIEvent.EventType == EGET_DIRECTORY_SELECTED)
            {
                if (validateDataLoc(
                    ((IGUIFileOpenDialog*)event.GUIEvent.Caller)->getDirectoryName()))
                {
                    m_valid_data_dir = true;
                    m_tex_sel = TexSel::getTexSel();
                    m_toolbox = ToolBox::getToolBox();
                    ((IGUIFileOpenDialog*)event.GUIEvent.Caller)->remove();
                }
                return true;
            }
            if (event.GUIEvent.EventType == EGET_FILE_CHOOSE_DIALOG_CANCELLED)
            {
                dataDirLocDlg();
                return true;
            }
        }
        return false;
    }


    if (event.EventType == EET_GUI_EVENT
        && event.GUIEvent.EventType == EGET_FILE_SELECTED)
    {
        closeTrack();
        m_device->getFileSystem()->changeWorkingDirectoryTo(m_def_wd);
        m_viewport->setTrack(new Track(path(((IGUIFileOpenDialog*)
            event.GUIEvent.Caller)->getFileName()).c_str()));
        m_viewport->setSplineMode(false);
        m_viewport->setState(Viewport::SELECT);
        RoadPanel::getRoadPanel()->updateRoadList();
        return true;
    }

    if (!m_viewport->getTrack()) return false;
     
    if (event.EventType == EET_GUI_EVENT)
    {
        RoadPanel* rp;
        s32 id = event.GUIEvent.Caller->getID();
        switch (event.GUIEvent.EventType)
        {
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
            case RoadPanel::DL_SELECT:
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
            case TerrPanel::INTENSITY:
            case TerrPanel::RADIUS:
                TerrPanel::getTerrPanel()->refreshTerrModData();
                m_viewport->setState(Viewport::TERRAIN_MOD);
                return true;
            case RoadPanel::DL_DETAIL:
                rp = RoadPanel::getRoadPanel();
                m_viewport->getTrack()->getRoadByID(rp->getSelectedRoadID())
                                                ->setDetail(rp->getDetail());
                return true;
            case RoadPanel::DL_WIDTH:
                rp = RoadPanel::getRoadPanel();
                m_viewport->getTrack()->getRoadByID(rp->getSelectedRoadID())
                                                 ->setWidth(rp->getWidth());
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
            default:
                break;
            }

        default:
            return false;
        }
    } // EventType == EET_GUI_EVENT

    // gui active, there is nothing we should do
    if (m_gui_env->getFocus() != NULL) return false;
    

    // keyboard event
    if (event.EventType == EET_KEY_INPUT_EVENT)
    {
        m_keys.keyEvent(event.KeyInput.Key, event.KeyInput.PressedDown);
        return true;
    }

    // mouse event
    if (event.EventType == EET_MOUSE_INPUT_EVENT)
    {
        // check if mouse is outside of the viewport's domain
        if (event.MouseInput.Y < 50 ||
            event.MouseInput.X >(s32) m_screen_size.Width - 250 ||
            (event.MouseInput.X >(s32)m_screen_size.Width - 500 && m_tex_sel->isActive()))
        {
            m_viewport->looseFocus();
            m_mouse.in_view = false;
            return false;
        }
        m_mouse.in_view = true;
        m_viewport->gainFocus();
        m_mouse.refresh(event);
    }
	return false;

} // OnEvent

// ----------------------------------------------------------------------------
void Editor::newTrack()
{
    closeTrack();
    f32 size = (f32)m_new_dialog_wndw->getSize();
    Track* t = new Track(size, size);
    t->setFileName(m_new_dialog_wndw->getFileName());
    t->setTrackName(m_new_dialog_wndw->getTrackName());
    t->setDesigner(m_new_dialog_wndw->getDesigner());
    m_viewport->setTrack(t);

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
    IFileSystem* file_system = m_device->getFileSystem();
    std::list<stringc> list;
    list = readRecentlyOpenedList();

    if (list.size() >= EVIL_NUMBER)
        list.pop_back();
    list.push_front(name);

    
    std::ofstream f;
    f.open((m_config_loc + "/recent.xml").c_str());
    
    f << "<files>/n";

    std::list<stringc>::iterator it = list.begin();
    for (; it != list.end(); it++)
    {
        f << "  <file name=\"" << (*it).c_str() << "\" />/n";
    }

    f << "</files>/n";

    f.close();
} // addToRecentlyOpenedList

// ----------------------------------------------------------------------------
std::list<stringc> Editor::readRecentlyOpenedList()
{
    std::list<stringc> list;

    IFileSystem* file_system = m_device->getFileSystem();
    IXMLReader*  xml_reader = file_system->createXMLReader("recent.xml");

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
ITexture* Editor::loadImg(const stringw& file_path)
{
    return m_editor->m_video_driver->getTexture(file_path);
} // loadImg

