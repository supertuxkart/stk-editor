#include "editor.hpp"

#include "viewport/viewport.hpp"
#include "viewport/indicator.hpp"
#include "gui/toolbar.hpp"

#include "gui/toolbox.hpp"
#include "gui/env_panel.hpp"
#include "gui/terr_panel.hpp"
#include "gui/road_panel.hpp"

#include "mesh/driveline.hpp"

#include <sstream>
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
    case ToolBar::TBI_NEW:
        RoadPanel::getRoadPanel()->getDriveLine()->exprt();
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
        m_viewport->setState(Viewport::TERRAIN_MOD);
        return true;
    // ToolBox / RoadPanel buttons:
    case RoadPanel::DL_CREATE:
        RoadPanel::getRoadPanel()->btnDown(ID);
        break;
    case RoadPanel::DL_ADD:
    case RoadPanel::DL_INSERT:
        m_viewport->setSplineMode(true);
        m_viewport->setState(Viewport::SPLINE);
        RoadPanel::getRoadPanel()->btnDown(ID);
        return true;
    case RoadPanel::DL_EXIT:
        m_viewport->setState(Viewport::SELECT);
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

    std::cerr << "Button click isn't handled!" << std::endl;
    return false;
}

// ----------------------------------------------------------------------------
bool Editor::init()
{
	m_device = createDevice(EDT_OPENGL, m_screen_size, 16, false, false, true);
	if (!m_device) return false;

	m_device->setResizable(true);
	m_device->setWindowCaption(L"SuperTuxKart Track Editor");

	m_video_driver  = m_device->getVideoDriver();
	m_scene_manager = m_device->getSceneManager();
	m_gui_env       = m_device->getGUIEnvironment();

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
    m_viewport->setFreeCamera(cam);

    // viewport init
    ICameraSceneNode* norm_cam;
    norm_cam = m_scene_manager->addCameraSceneNode(0, vector3df(25, 50, 30),
        vector3df(25, -30, -15));
    norm_cam->setID(2);
    m_viewport = Viewport::get(norm_cam, &m_mouse, &m_keys);
    m_indicator = m_viewport->getIndicator();
    m_scene_manager->setActiveCamera(norm_cam);

    m_toolbar = ToolBar::getToolBar();
    m_toolbox = ToolBox::getToolBox();

    m_device->setEventReceiver(this);
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
        m_viewport->animate(current_time - last_time);

		// drawing
		m_video_driver->beginScene(true, true, SColor(255, 80, 0, 170));

        m_scene_manager->drawAll();
		m_gui_env->drawAll();

        m_indicator->render();

		m_video_driver->endScene();

        if (m_video_driver->getScreenSize() != m_screen_size)
        {
            m_screen_size = m_video_driver->getScreenSize();
            m_toolbar->reallocate();
            m_toolbox->reallocate();
        }
        last_time = current_time;
    }

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
                m_viewport->setState(Viewport::TERRAIN_MOD);
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
                m_viewport->setState(Viewport::TERRAIN_MOD);
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
        m_keys.keyEvent(event.KeyInput.Key, event.KeyInput.PressedDown);
        return true;
    }

    // mouse event
    if (event.EventType == EET_MOUSE_INPUT_EVENT)
    {
        // check if mouse is outside of the viewport's domain
        if (event.MouseInput.Y < 50 ||
            event.MouseInput.X >(s32) m_screen_size.Width - 250)
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
ITexture* Editor::loadImg(const stringw& file_path)
{
    return m_editor->m_video_driver->getTexture(file_path);
} // loadImg

