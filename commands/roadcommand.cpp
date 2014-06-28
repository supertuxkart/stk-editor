#include "commands/roadcommand.hpp"

#include "road/road.hpp"


// ----------------------------------------------------------------------------
RoadCommand::RoadCommand(IRoad* road, bool insert) :m_road(road), m_insert(insert) 
{
    m_created = false;
    m_spline  = m_road->getSpline(); 
    m_cp.node = 0;
}; // RoadCommand

// ----------------------------------------------------------------------------
void RoadCommand::updatePos(vector3df pos)
{
    m_pos = pos;
    if (!m_created)
    {
        if (m_insert) m_ix = m_spline->insertControlPoint(pos);
        else m_ix = m_spline->addControlPoint(pos);
        m_created = true;
    }
    else
    {
        if (m_insert)
        {
            m_spline->removeControlPoint(m_ix);
            m_ix = m_spline->insertControlPoint(pos);
        }
        else m_spline->getNode(m_ix)->setPosition(pos);
    }

    m_spline->updatePosition();
    m_road->refresh();

} // updatePos

// ----------------------------------------------------------------------------
void RoadCommand::redo()
{
    if (m_insert)
    {
        m_spline->addControlPoint(m_cp, m_ix);
    }
    else
    {
        m_spline->addControlPoint(m_cp);
    }
    m_road->refresh();
    m_created = true;
}

// ----------------------------------------------------------------------------
void RoadCommand::undo()
{
    m_cp = m_spline->removeControlPoint(m_ix);
    m_road->refresh();
    m_created = false;
}
