#ifndef TERRPANEL_HPP
#define TERRPANEL_HPP

#include <irrlicht.h>

#include "entities/terrain.hpp"

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
        H_EDGE_1 = FGEI,
        H_EDGE_2,
        H_EDGE_3,
        H_RADIUS,
        H_INTENSITY
    };

private:

    // private variables:

    static TerrPanel*       m_terr_panel;

    IGUITab*                m_tab;

    IGUIScrollBar*          m_h_radius;
    IGUIScrollBar*          m_h_intensity;

    IGUIImage*              m_active_edge_frame;
    
    Terrain::TerrainMod     m_tmod;

    // private functions:

    TerrPanel() {};

    void init();

public:
    static TerrPanel*     getTerrPanel(IGUITab* tab = 0);

    Terrain::TerrainMod*  getTerrainModData()         { return &m_tmod; }
    void                  btnDown(int btn);
    void                  refreshTerrModData();
};

#endif
