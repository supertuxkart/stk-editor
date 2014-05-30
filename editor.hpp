#ifndef EDITOR_HPP
#define EDITOR_HPP

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace gui;

class ToolBar;

class Editor :public IEventReceiver
{
private:
	static Editor*              m_editor;

	IrrlichtDevice*             m_device;
	IVideoDriver*               m_video_driver;
	ISceneManager*              m_scene_manager;
	IGUIEnvironment*            m_gui_env;

    ToolBar*                    m_toolbar;

	bool init();

	Editor() {};

public:
	static Editor*    getEditor();
	bool		      run();
	virtual bool      OnEvent(const SEvent& event);

    IVideoDriver*     getVideoDriver() { return m_video_driver; }
    IGUIEnvironment*  getGUIEnv()      { return m_gui_env;      }
};

#endif
