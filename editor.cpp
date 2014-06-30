#include "editor.hpp"

#include "track.hpp"
#include "indicator.hpp"
#include "toolbar.hpp"

#include "toolbox/toolbox.hpp"
#include "toolbox/envpanel.hpp"
#include "toolbox/terrpanel.hpp"
#include "toolbox/roadpanel.hpp"

#include <iostream>

Editor* Editor::m_editor = 0;

// ----------------------------------------------------------------------------
bool Editor::buttonClicked(int ID)
{
    switch (ID)
    {
    // ToolBar buttons
    case ToolBar::TBI_EXIT:
        m_device->closeDevice();
        return true;
    case ToolBar::TBI_UNDO:
        m_track->undo();
        return true;
    case ToolBar::TBI_REDO:
        m_track->redo();
        return true;
    case ToolBar::TBI_SELECT:
        m_track->setState(Track::SELECT);
        return true;
    case ToolBar::TBI_MOVE:
        m_track->setState(Track::MOVE);
        return true;
    case ToolBar::TBI_ROTATE:
        m_track->setState(Track::ROTATE);
        return true;
    case ToolBar::TBI_SCALE:
        m_track->setState(Track::SCALE);
        return true;
    case ToolBar::TBI_DELETE:
        m_track->deleteCmd();
        return true;
    case ToolBar::TBI_CAM:
        m_track->setState(Track::FREECAM);
        return true;
    case ToolBar::TBI_SPLINE:
        m_track->setSplineMode(!m_track->getSplineMode());
        return true;
    case ToolBar::TBI_NEW:
        m_track->setSplineMode(false);
        return true;
    // ToolBox BTN:
    case ToolBox::TWND_ID:
    case ToolBox::EWND_ID:
    case ToolBox::RWND_ID:
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
        TerrPanel::getTerrPanel()->btnDown(ID);
        m_track->setState(Track::TERRAIN_MOD);
        return true;
    // ToolBox / RoadPanel buttons:
    case RoadPanel::DL_CREATE:
        RoadPanel::getRoadPanel()->btnDown(ID);
        break;
    case RoadPanel::DL_ADD:
    case RoadPanel::DL_INSERT:
        m_track->setSplineMode(true);
        m_track->setState(Track::SPLINE);
        RoadPanel::getRoadPanel()->btnDown(ID);
        return true;
    case RoadPanel::DL_EXIT:
        m_track->setState(Track::SELECT);
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
        m_track->setSplineMode(false);
        m_track->setNewEntity(EnvPanel::getEnvPanel()->getModelPathFromBtnId(ID));
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
    
    std::cerr << "Button click isn't handled!" << std::endl;
    return false;    
}

// ----------------------------------------------------------------------------
bool Editor::init()
{
	m_device = createDevice(EDT_OPENGL,
            m_screen_size, 16,
		    false, false, true);

	if (!m_device) return false;

	m_device->setResizable(true);

	m_device->setWindowCaption(L"SuperTuxKart Track Editor");

	m_video_driver  = m_device->getVideoDriver();
	m_scene_manager = m_device->getSceneManager();
	m_gui_env       = m_device->getGUIEnvironment();

    IGUISkin* skin = m_gui_env->getSkin();
    IGUIFont* font = m_gui_env->getFont(L"font/font2.png");
    skin->setFont(font);
    
    
    m_scene_manager->setAmbientLight(SColorf(0.3f, 0.3f, 0.3f, 1.0f));
    ILightSceneNode* l = m_scene_manager->addLightSceneNode(0, vector3df(0, 1, 0), 
                                              SColorf(1.0f, 1.0f, 1.0f), 500, -1);
    l->setLightType(ELT_DIRECTIONAL);
    l->setPosition(vector3df(0, 1, 0));

    for (s32 i = 0; i<EGDC_COUNT; ++i)
    {
        video::SColor col = skin->getColor((EGUI_DEFAULT_COLOR)i);
        col.setAlpha(255);
        skin->setColor((EGUI_DEFAULT_COLOR)i, col);
    }


    ICameraSceneNode* norm_cam;
    norm_cam = m_scene_manager->addCameraSceneNode(0, vector3df(25, 50, 30),
                                                 vector3df(25, -30, -15));
    norm_cam->setID(2);

    m_track = Track::getTrack(norm_cam);
    
    ICameraSceneNode* cam;
    cam = m_scene_manager->addCameraSceneNodeMaya();
    cam->setID(1);
    cam->setFarValue(20000.f);
    cam->setTarget(vector3df(0, 0, 0));
    cam->setInputReceiverEnabled(false);
    m_track->setFreeCamera(cam);

    m_toolbar = ToolBar::getToolBar();
    m_toolbox = ToolBox::getToolBox();

    m_scene_manager->setActiveCamera(norm_cam);
    m_device->setEventReceiver(this);

    m_indicator = m_track->getIndicator();

    return true;
} // init


// ----------------------------------------------------------------------------
Editor* Editor::getEditor(dimension2du screen_size)
{
	if (m_editor != 0) return m_editor;

	m_editor = new Editor();
    m_editor->m_screen_size = screen_size;
	if (!m_editor->init()) return 0;
	return m_editor;
} // getEditor

// ----------------------------------------------------------------------------
bool Editor::run()
{
	if (!m_device) return 0;


    long current_time = m_device->getTimer()->getTime();
    long last_time    = current_time;


	while (m_device->run())
    {

        current_time = m_device->getTimer()->getTime();
        m_track->animate(current_time - last_time);

		// drawing
		m_video_driver->beginScene(true, true, SColor(255, 80, 0, 170));

        m_indicator->render();

		m_scene_manager->drawAll();
		m_gui_env->drawAll();

        m_video_driver->draw2DImage(m_indicator->getTexture(), 
            position2d<s32>(0, m_screen_size.Height - 300), rect<s32>(0, 0, 300, 300), (rect<s32>*)0,
            SColor(255, 255, 255, 255),true);


		m_video_driver->endScene();

        if (m_video_driver->getScreenSize() != m_screen_size)
        {
            m_screen_size = m_video_driver->getScreenSize();
            m_toolbar->reallocate();
            m_toolbox->reallocate();
        }

        last_time = current_time;
    }

    delete m_track;
    delete m_toolbox;
    delete m_toolbar;

    // crash ??? 
    // m_device->drop();

	return 1;
} // run

// ----------------------------------------------------------------------------
bool Editor::OnEvent(const SEvent& event)
{
    if (event.EventType == EET_GUI_EVENT)
    {
        s32 id = event.GUIEvent.Caller->getID();

        if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
        {
            m_gui_env->removeFocus(m_gui_env->getFocus()->getParent());
            return buttonClicked(id);
        }
        else
        if (event.GUIEvent.EventType == EGET_COMBO_BOX_CHANGED)
        {
            switch (event.GUIEvent.Caller->getID())
            {
            case EnvPanel::CB_ID:
                EnvPanel::getEnvPanel()->resetIndex();
                EnvPanel::getEnvPanel()->refreshBtnTable();
                return true;
            case RoadPanel::DL_SELECT:
                RoadPanel::getRoadPanel()->select();
                break;
            default:
                break;
            }
        } // GUIEvent.EventType == EGET_COMBO_BOX_CHANGED
        if (event.GUIEvent.EventType == EGET_EDITBOX_CHANGED)
        {
            switch (event.GUIEvent.Caller->getID())
            {
            case EnvPanel::SF_ID:
                EnvPanel::getEnvPanel()->resetIndex();
                EnvPanel::getEnvPanel()->refreshBtnTable();
                return true;
            default:
                break;
            }
        }
        if (event.GUIEvent.EventType == EGET_SCROLL_BAR_CHANGED)
        {
            switch (event.GUIEvent.Caller->getID())
            {
            case TerrPanel::INTENSITY:
            case TerrPanel::RADIUS:
                TerrPanel::getTerrPanel()->refreshTerrModData();
                m_track->setState(Track::TERRAIN_MOD);
                return true;
            case RoadPanel::DL_DETAIL:
            case RoadPanel::DL_WIDTH:
                RoadPanel::getRoadPanel()->scrollBarChanged();
                return true;
            default:
                break;
            }
        }
        if (event.GUIEvent.EventType == EGET_CHECKBOX_CHANGED)
        {
            switch (event.GUIEvent.Caller->getID())
            {
            case TerrPanel::H_MAX_CHECK_BOX:
            case TerrPanel::H_MIN_CHECK_BOX:
                TerrPanel::getTerrPanel()->refreshTerrModData();
                m_track->setState(Track::TERRAIN_MOD);
                return true;
            default:
                break;
            }
        }
    } // EventType == EET_GUI_EVENT

    // gui active, there is nothing we should do
    if (m_gui_env->getFocus() != NULL) return false;

    // keyboard event
    if (event.EventType == EET_KEY_INPUT_EVENT)
    {
        m_track->keyEvent(event.KeyInput.Key, event.KeyInput.PressedDown);
        return true;
    }

    // mouse event
    if (event.EventType == EET_MOUSE_INPUT_EVENT)
    {
        m_track->mouseEvent(event);
    }

	return false;

} // OnEvent

// ----------------------------------------------------------------------------
ITexture* Editor::loadImg(const stringw& file_path)
{
    return m_editor->m_video_driver->getTexture(file_path);
} // loadImg
