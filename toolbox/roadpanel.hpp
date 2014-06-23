#ifndef ROADPANEL_HPP
#define ROADPANEL_HPP

#include <irrlicht.h>


using namespace irr;
using namespace gui;
using namespace core;

class RoadPanel
{
public:
    // first gui element id
    static const int FGEI = 400;

    enum
    {
        DL_ADD = FGEI,
        DL_INSERT,
        DL_EXIT
    };

private:

    // private variables:

    static RoadPanel*       m_road_panel;

    IGUIWindow*             m_wndw;

    bool                    m_insert = false;

    // private functions:

    RoadPanel() {};

    void init();

public:
    static RoadPanel*     getRoadPanel(IGUIWindow* wndw = 0);
    void                  btnDown(int btn);
    bool                  isInsertMode() { return m_insert; }
};

#endif
