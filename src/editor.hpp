#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "input/keys.hpp"
#include "input/mouse.hpp"

#include <irrlicht.h>

#define MAGIC_NUMBER            8192
#define ANOTHER_MAGIC_NUMBER    16384

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace gui;
using namespace io;

class Indicator;
class  ToolBar;
class  ToolBox;
class  NewDialogWndw;
class  Viewport;

class Editor :public IEventReceiver
{
private:
	static Editor*              m_editor;

	IrrlichtDevice*             m_device;
	IVideoDriver*               m_video_driver;
	ISceneManager*              m_scene_manager;
	IGUIEnvironment*            m_gui_env;

    ToolBar*                    m_toolbar;
    ToolBox*                    m_toolbox;
    NewDialogWndw*              m_new_dialog_wndw;
    Viewport*                   m_viewport;

    path                        m_def_wd;

    dimension2du                m_screen_size;

    Mouse                       m_mouse;
    Keys                        m_keys;
    Indicator*                  m_indicator;

	bool init();

	Editor() {};
    bool                        buttonClicked(int ID);

public:
    static Editor*    getEditor(dimension2du screen_size = dimension2du(1280, 720));
	bool		      run();
	virtual bool      OnEvent(const SEvent& event);
    void              newTrack();
    void              closeTrack();

    static ITexture*  loadImg(const stringw& file_path);

    IrrlichtDevice*   getDevice()       { return m_device;        }
    IVideoDriver*     getVideoDriver()  { return m_video_driver;  }
    ISceneManager*    getSceneManager() { return m_scene_manager; }
    IGUIEnvironment*  getGUIEnv()       { return m_gui_env;       }
    dimension2du      getScreenSize()   { return m_screen_size;   }
};

#endif
