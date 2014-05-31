#ifndef TOOLBOX_HPP
#define TOOLBOX_HPP

#include <irrlicht.h>

using namespace irr;
using namespace gui;

class ToolBox
{
private:

    // private variables:

    static ToolBox* m_toolbox;

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
