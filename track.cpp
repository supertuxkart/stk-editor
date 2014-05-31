#include "track.hpp"

Track* Track::m_track = 0;

// ----------------------------------------------------------------------------
Track* Track::getTrack()
{
    if (m_track != 0) return m_track;

    m_track = new Track();
    m_track->init();
    return m_track;
} // getTrack


// ----------------------------------------------------------------------------
void Track::init()
{
    m_state = SELECT;
    m_grid_on = true;
} // init

// ----------------------------------------------------------------------------
void Track::setState(State state)
{
    m_state = state;
} // setState

// ----------------------------------------------------------------------------
void Track::setGrid(bool grid_on)
{
    m_grid_on = grid_on;
} // setGrid

// ----------------------------------------------------------------------------
void Track::changeGridDensity(int dir)
{

} // changeGridDensity
