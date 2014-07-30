#ifndef RCCOMMAND_HPP
#define RCCOMMAND_HPP

#include "commands/icmd.hpp"

#include "mesh/road.hpp"
#include "gui/road_panel.hpp"

#include <irrlicht.h>

using namespace irr;
using namespace core;

// ----------------------------------------------------------------------------
class CreateRoadCmd :public ICmd
{
private:
    IRoad*  m_road;
    bool    m_done;
    bool    m_inverted;
public:
    CreateRoadCmd(IRoad* r, bool inv = false) :m_road(r),m_inverted(inv)
                                              { m_done = true; }

    // ----------------------------------------------------------------------------
    virtual ~CreateRoadCmd() 
    { 
        if (!m_done)
        {
            m_road->getSpline()->remove();
            m_road->remove();
        }
    } // ~createRoadCmd

    // ----------------------------------------------------------------------------
    virtual void redo()
    { 
        if (m_inverted)
        {
            m_inverted = false;
            undo();
            m_inverted = true;
            return;
        }
        m_done = true;
        m_road->setVisible(true);
        m_road->getSpline()->setVisible(true);
        Viewport::get()->getTrack()->insertRoad(m_road);
        RoadPanel::getRoadPanel()->updateRoadList();
        Viewport::get()->setActiveRoad(m_road);
    } // redo

    // ----------------------------------------------------------------------------
    virtual void undo()
    {
        if (m_inverted)
        {
            m_inverted = false;
            redo();
            m_inverted = true;
            return;
        }
        m_done = false;
        m_road->setVisible(false);
        m_road->getSpline()->setVisible(false);
        Viewport::get()->getTrack()->removeRoad(m_road);
        RoadPanel::getRoadPanel()->updateRoadList();
    } // undo
};
#endif

