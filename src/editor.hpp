#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "input/keys.hpp"
#include "input/mouse.hpp"

#include <irrlicht.h>
#include <list>

#define MAGIC_NUMBER            8192
#define ANOTHER_MAGIC_NUMBER    16384

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace gui;
using namespace io;

class  Indicator;
class  ToolBar;
class  ToolBox;
class  NewDialogWndw;
class  WelcomeScreen;
class  Viewport;
class  TexSel;

class Editor :public IEventReceiver
{
private:
    static const int            EVIL_NUMBER = 5;
	static Editor*              m_editor;

	IrrlichtDevice*             m_device;
	IVideoDriver*               m_video_driver;
	ISceneManager*              m_scene_manager;
	IGUIEnvironment*            m_gui_env;

    ToolBar*                    m_toolbar;
    ToolBox*                    m_toolbox;
    NewDialogWndw*              m_new_dialog_wndw;
    WelcomeScreen*              m_welcome_screen;
    Viewport*                   m_viewport;

    TexSel*                     m_tex_sel;

    bool                        m_valid_data_dir;

    path                        m_def_wd;

    dimension2du                m_screen_size;

    Mouse                       m_mouse;
    Keys                        m_keys;
    Indicator*                  m_indicator;

    path                        m_config_loc;
    IFileArchive*               m_tex_dir;
    IFileArchive*               m_xml_dir;
    c8*                         m_maps_path;
    path                        m_track_dir;

	bool                        init();
    void                        fileInit();
    bool                        validDataLoc(IXMLReader* xml);
    bool                        validateDataLoc(path data_loc);
    void                        dataDirLocDlg();

	Editor() {};
    bool                        buttonClicked(int ID);
    bool                        importantButtonClicked(int ID);

public:
    static Editor*              getEditor(dimension2du screen_size = dimension2du(1280, 720));
	bool		                run();
	virtual bool                OnEvent(const SEvent& event);
    void                        open(path p);
    void                        newTrack();
    void                        closeTrack();
    void                        addToRecentlyOpenedList(stringc name);
    std::list<stringc>          readRecentlyOpenedList();

    void                        addItem(u32 id);

    static ITexture*            loadImg(const stringw& file_path);

    IrrlichtDevice*             getDevice()       { return m_device;        }
    IVideoDriver*               getVideoDriver()  { return m_video_driver;  }
    ISceneManager*              getSceneManager() { return m_scene_manager; }
    IGUIEnvironment*            getGUIEnv()       { return m_gui_env;       }
    dimension2du                getScreenSize()   { return m_screen_size;   }
    IFileArchive*               getTexDir()       { return m_tex_dir;       }
    IFileArchive*               getXMLDir()       { return m_xml_dir;       }
    path                        getMapsPath()     { return m_maps_path;     }
    path                        getTrackDir()     { return m_track_dir;     }
};

#endif
