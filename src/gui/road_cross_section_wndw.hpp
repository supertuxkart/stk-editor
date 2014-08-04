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
private:

    // private variables:
    static RoadCrossSectionWndw*   m_self;

    ISceneManager*                 m_smgr;
    IVideoDriver*                  m_driver;

    s32 static const               m_offset = -3333;
    ICameraSceneNode*              m_cam;

    Road*                          m_road;
    ISceneNode**                   m_nodes;
    u32                            m_node_num;

    bool                           m_visible;

    // private functions:

    void    init();

    RoadCrossSectionWndw() {};
public:
    static RoadCrossSectionWndw*   get();

    void                           show(Road* r);
    void                           hide();

    void                           render();

    bool                           isVisible() { return m_visible; }

};

#endif
