#ifndef RCCOMMAND_HPP
#define RCCOMMAND_HPP

#include "commands/icmd.hpp"

#include "mesh/road.hpp"
#include "gui/road_panel.hpp"

#include <irrlicht.h>

using namespace irr;
using namespace core;

// ----------------------------------------------------------------------------
class createRoadCmd :public ICmd
{
private:
    IRoad*  m_road;
    bool    m_done;
public:
    createRoadCmd(IRoad* r, stringw name) :m_road(r) { m_done = true; };

    virtual ~createRoadCmd() { if (!m_done) m_road->remove(); }

    // ----------------------------------------------------------------------------
    virtual void redo()
    { 
        m_done = true;
        m_road->setVisible(true);
        m_road->getSpline()->setVisible(true);
        Viewport::get()->getTrack()->insertRoad(m_road);
        RoadPanel::getRoadPanel()->updateRoadList();
    }

    // ----------------------------------------------------------------------------
    virtual void undo()
    {
        m_done = false;
        m_road->setVisible(false);
        m_road->getSpline()->setVisible(false);
        Viewport::get()->getTrack()->removeLastRoad();
        RoadPanel::getRoadPanel()->updateRoadList();
    }
};

#endif

