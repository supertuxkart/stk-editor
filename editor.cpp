#include "editor.hpp"

#include "toolbar.hpp"

Editor* Editor::m_editor = 0;

// ----------------------------------------------------------------------------
bool Editor::init()
{
	m_device = createDevice(video::EDT_OPENGL,
		dimension2d<u32>(1024, 768), 16,
		false, false, true, 0);

	if (!m_device) return false;
	m_device->setResizable(true);

	m_device->setWindowCaption(L"SuperTuxKart Track Editor");

	m_video_driver  = m_device->getVideoDriver();
	m_scene_manager = m_device->getSceneManager();
	m_gui_env       = m_device->getGUIEnvironment();

	m_scene_manager->addCameraSceneNode(0,
		vector3df(0, 0, -50),
		vector3df(0, 0, 50)
		);

    m_toolbar = ToolBar::getToolBar();

    return true;
} // init


// ----------------------------------------------------------------------------
Editor* Editor::getEditor()
{
	if (m_editor != 0) return m_editor;

	m_editor = new Editor();
	if (!m_editor->init()) return 0;
	return m_editor;
} // getEditor

// ----------------------------------------------------------------------------
bool Editor::run()
{
	if (!m_device) return 0;

    // temporary stuff: editor should read screen size from xml and store it
    unsigned int last_screen_x = 0;

	while (m_device->run())
    {
		// drawing
		m_video_driver->beginScene(true, true, SColor(255, 80, 0, 170));

		m_scene_manager->drawAll();
		m_gui_env->drawAll();
		
		m_video_driver->endScene();

        if (m_video_driver->getScreenSize().Width != last_screen_x)
        {
            last_screen_x = m_video_driver->getScreenSize().Width;
            m_toolbar->reallocate(last_screen_x);
        }

    }
	return 1;
} // run

// ----------------------------------------------------------------------------
bool Editor::OnEvent(const SEvent& event)
{
   

	return false;
} // OnEvent
