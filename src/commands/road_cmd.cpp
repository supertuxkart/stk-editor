#include "commands/road_cmd.hpp"

#include "mesh/iroad.hpp"
#include "spline/ispline.hpp"

// ----------------------------------------------------------------------------
RoadCmd::RoadCmd(IRoad* road, bool insert) :m_road(road), m_insert(insert)
{
    m_created = false;
    m_spline  = m_road->getSpline(); 
    m_cp.node = 0;
}; // RoadCommand

// ----------------------------------------------------------------------------
void RoadCmd::updatePos(vector3df pos)
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
void RoadCmd::redo()
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
void RoadCmd::undo()
{
    m_cp = m_spline->removeControlPoint(m_ix);
    m_road->refresh();
    m_created = false;
}
