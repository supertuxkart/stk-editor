#ifndef TOOLBOX_HPP
#define TOOLBOX_HPP

#include <irrlicht.h>

class EnvPanel;
class TerrPanel;
class RoadPanel;
class ExtraPanel;

using namespace irr;
using namespace gui;
using namespace core;

class ToolBox
{
public:
    static const int TBOX_ID     = 100;
    static const int TWND_ID     = 101;
    static const int EWND_ID     = 102;
    static const int RWND_ID     = 103;
    static const int XWND_ID     = 104;
private:

    // private variables:

    static ToolBox* m_toolbox;

    EnvPanel*       m_env_panel;
    TerrPanel*      m_terr_panel;
    RoadPanel*      m_road_panel;
    ExtraPanel*     m_extra_panel;

    IGUIWindow*     m_wndw;
    
    IGUIWindow*     m_terr_wndw;
    IGUIWindow*     m_env_wndw;
    IGUIWindow*     m_road_wndw;
    IGUIWindow*     m_extr_wndw;

    // private functions:

    void    init();

    ToolBox() {};

protected:
    void                initWndw(IGUIWindow* wndw);

public:
    static ToolBox*     getToolBox();
    void                reallocate();
    void                setWndw(int ID);
};

#endif
