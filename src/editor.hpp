#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "input/keys.hpp"
#include "input/mouse.hpp"

#include <irrlicht.h>
#include <list>

#define _(a) a
#define MAGIC_NUMBER            8192
#define ANOTHER_MAGIC_NUMBER    12288

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
class  MsgWndw;
class  RoadCrossSectionWndw;

class Editor :public IEventReceiver
{
private:
    static const u32            EVIL_NUMBER = 5;
    static Editor*              m_editor;
    
    IrrlichtDevice*             m_device;
    IVideoDriver*               m_video_driver;
    ISceneManager*              m_scene_manager;
    IGUIEnvironment*            m_gui_env;

    ToolBar*                    m_toolbar;
    ToolBox*                    m_toolbox;
    NewDialogWndw*              m_new_dialog_wndw;
    WelcomeScreen*              m_welcome_screen;
    MsgWndw*                    m_msg_wndw;
    Viewport*                   m_viewport;

    TexSel*                     m_tex_sel;
    RoadCrossSectionWndw*       m_rcs;

    IGUIFont*                   m_font;

    bool                        m_valid_data_dir;

    path                        m_def_wd;

    dimension2du                m_screen_size;

    Mouse                       m_mouse;
    Keys                        m_keys;
    Indicator*                  m_indicator;

    path                        m_data_loc;
    path                        m_config_loc;
    path                        m_icons_loc;
    IFileArchive*               m_tex_dir;
    IFileArchive*               m_xml_dir;
    path                        m_track_dir;
    c8*                         m_maps_path;
    c8*                         m_music_loc;
    path                        m_exe_loc;

    bool                        init();
    void                        initAfterDataDirKnown();
    void                        readConfigFile(IFileSystem* file_system);
    void                        fileInit();
    void                        initResolFromConfigFile();
    void                        writeResAndExePathIntoConfig();
    bool                        isValidDataLoc();
    bool                        validateDataLoc(path data_loc);
    void                        initDataLoc();
    void                        dataDirLocDlg();
    void                        runTrack();

    Editor() {};
    bool                        buttonClicked(int ID);
    bool                        importantButtonClicked(int ID);
    void                        simpleShortcuts(EKEY_CODE code);
    void                        shiftShortcuts(EKEY_CODE code);
    void                        ctrlShortcuts(EKEY_CODE code);
public:
    static bool                 isValidSize(u8 size);

    static Editor*              getEditor(dimension2du screen_size = dimension2du(1280, 720));
    void                        render();
    bool                        run();
    virtual bool                OnEvent(const SEvent& event);
    void                        keepMouseIn(s32 sx, s32 sy);
    bool                        open(path p);
    void                        newTrack();
    void                        closeTrack();
    void                        addToRecentlyOpenedList(stringc name);
    std::list<stringc>          readRecentlyOpenedList();

    void                        addItem(u32 id);
    static path                 toRelative(path p);

    static ITexture*            loadImg(const stringw& file_path);

    static stringc              getTexStr(ITexture* tex);
    static void                 writeStrc(FILE* fp, stringc str);
    static void                 readTexSt(FILE* fp, ITexture** tex);
    static stringc              getLib(stringc s);

    IrrlichtDevice*             getDevice()       { return m_device;        }
    IVideoDriver*               getVideoDriver()  { return m_video_driver;  }
    ISceneManager*              getSceneManager() { return m_scene_manager; }
    IGUIEnvironment*            getGUIEnv()       { return m_gui_env;       }
    dimension2du                getScreenSize()   { return m_screen_size;   }
    IGUIFont*                   getFont()         { return m_font;          }
    IFileArchive*               getTexDir()       { return m_tex_dir;       }
    IFileArchive*               getXMLDir()       { return m_xml_dir;       }
    path                        getMapsPath()     { return m_maps_path;     }
    path                        getTrackDir()     { return m_track_dir;     }
    path                        getIconsLoc()     { return m_icons_loc;     }

};

#endif
