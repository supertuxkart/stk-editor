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
    static const int TBOX_ID     = 1112;
private:

    // private variables:

    static ToolBox* m_toolbox;

    EnvPanel*       m_env_panel;
    TerrPanel*      m_terr_panel;

    IGUIWindow*     m_boxwnd;
    IGUITabControl* m_tab;

    // private functions:

    void    init();

    ToolBox() {};

public:
    static ToolBox*     getToolBox();
    void                reallocate();
};

#endif
