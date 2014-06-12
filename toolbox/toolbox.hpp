#ifndef TOOLBOX_HPP
#define TOOLBOX_HPP

#include <irrlicht.h>

class EnvPanel;
class TerrPanel;

using namespace irr;
using namespace gui;
using namespace core;

class ToolBox
{
public:
    static const int TBOX_ID     = 100;
    static const int TWND_ID     = 101;
    static const int EWND_ID     = 102;
private:

    // private variables:

    static ToolBox* m_toolbox;

    EnvPanel*       m_env_panel;
    TerrPanel*      m_terr_panel;

    IGUIWindow*     m_wndw;
    
    IGUIWindow*     m_terr_wndw;
    IGUIWindow*     m_env_wndw;

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
