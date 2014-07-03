#ifndef TERRPANEL_HPP
#define TERRPANEL_HPP

#include <irrlicht.h>

#include "viewport/terrain.hpp"

using namespace irr;
using namespace gui;
using namespace core;

class TerrPanel
{
public:
    // first gui element id
    static const int FGEI = 200;
    
    enum
    {
        RADIUS = FGEI,
        INTENSITY,
        EDGE_1,
        EDGE_2,
        EDGE_3,
        M_T1,
        M_T2,
        M_T3,
        M_T4,
        H_MIN_CHECK_BOX,
        H_MAX_CHECK_BOX,
        H_MIN_EDIT_BOX,
        H_MAX_EDIT_BOX,
        H_BTN,
        T_SOFT_BTN,
        T_HARD_BTN,
        T_BRIGHTNESS_BTN
    };

private:

    // private variables:

    static TerrPanel*       m_terr_panel;

    IGUIWindow*             m_wndw;

    IGUIScrollBar*          m_h_radius;
    IGUIScrollBar*          m_h_intensity;

    IGUIImage*              m_active_edge_frame;

    IGUIEditBox*            m_h_min_value;
    IGUIEditBox*            m_h_max_value;

    IGUICheckBox*           m_h_min_cb;
    IGUICheckBox*           m_h_max_cb;
    
    TerrainMod              m_tmod;

    // private functions:

    TerrPanel() {};

    void init();

public:
    static TerrPanel*     getTerrPanel(IGUIWindow* wndw = 0);

    TerrainMod*           getTerrainModData()         { return &m_tmod; }
    void                  btnDown(int btn);
    void                  refreshTerrModData();
};

#endif
