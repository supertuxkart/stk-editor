#ifndef EXTRA_PANEL_HPP
#define EXTRA_PANEL_HPP

#include <irrlicht.h>

using namespace irr;
using namespace gui;
using namespace core;

class ExtraPanel
{
public:
    // first gui element id
    static const int FGEI = 510;
    enum
    {
        BTN_BANANA = FGEI,
        BTN_ITEM,
        BTN_SNITRO,
        BTN_BNITRO
    };

private:

    // private variables:

    static ExtraPanel*      m_extra_panel;

    IGUIWindow*             m_wndw;

    // private functions:

    ExtraPanel() {};

    void init();

public:
    static ExtraPanel*    getExtraPanel(IGUIWindow* wndw = 0);

    void                  btnDown(int btn);
    void                  reallocate(dimension2du ss);
};

#endif
