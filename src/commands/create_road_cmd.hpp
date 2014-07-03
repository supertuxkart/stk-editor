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
    stringw m_name;
    bool    m_done;
public:
    createRoadCmd(IRoad* r, stringw name) :m_road(r), m_name(name) { m_done = true; };

    virtual ~createRoadCmd() { if (!m_done) m_road->remove(); }

    // ----------------------------------------------------------------------------
    virtual void redo()
    { 
        m_done = true;
        m_road->setVisible(true);
        m_road->getSpline()->setVisible(true);
        RoadPanel::getRoadPanel()->restoreRoad(m_road, m_name);
    }

    // ----------------------------------------------------------------------------
    virtual void undo()
    {
        m_done = false;
        m_road->setVisible(false);
        m_road->getSpline()->setVisible(false);
        RoadPanel::getRoadPanel()->removeLastRoad();
    }
};

#endif

