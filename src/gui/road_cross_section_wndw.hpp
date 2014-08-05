#ifndef ROAD_CROSS_SECTION_WNDW_HPP
#define ROAD_CROSS_SECTION_WNDW_HPP

#include <irrlicht.h>

using namespace irr;
using namespace gui;
using namespace core;
using namespace scene;
using namespace video;

class Road;

class RoadCrossSectionWndw
{
public:
    static const int FIRST_BTN_ID = 700;

    enum
    {
        GRID_ON_OFF = FIRST_BTN_ID,
        GRID_P,
        GRID_M,
        POINT_P,
        POINT_M,
        SYM_ON_OFF,
        OK,
        CANCEL
    };

private:

    // private variables:
    static RoadCrossSectionWndw*   m_self;

    ISceneManager*                 m_smgr;
    IVideoDriver*                  m_driver;

    s32 static const               m_offset = -3333;
    ICameraSceneNode*              m_cam;

    IGUIButton*                    m_gof;
    IGUIButton*                    m_gp;
    IGUIButton*                    m_gm;
    IGUIButton*                    m_pp;
    IGUIButton*                    m_pm;
    IGUIButton*                    m_sym;
    IGUIButton*                    m_ok;
    IGUIButton*                    m_cancel;

    bool                           m_sym_mode;
    bool                           m_allign;

    Road*                          m_road;
    ISceneNode**                   m_nodes;
    u32                            m_node_num;

    ISceneNode*                    m_center_node;

    bool                           m_visible;

    f32                            m_grid;
    bool                           m_grid_on;
    f32                            m_rt;

    // private functions:

    void    init();
    void    setPointNum(u32 n);
    void    createNodesFromPoints(array<vector2df> points);
    void                           drawGrid();

    RoadCrossSectionWndw() {};
public:
    static RoadCrossSectionWndw*   get();

    void                           show(Road* r);
    void                           hide(bool apply_mod);

    void                           render();

    void                           animate(u32 dt, bool dirty = false);

    void                           buttonClicked(u32 id);
    void                           reallocate();

    bool                           isVisible() { return m_visible; }

};

#endif
