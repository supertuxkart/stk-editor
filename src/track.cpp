#include "track.hpp"
#include "editor.hpp"

#include "mesh/terrain.hpp"


// ----------------------------------------------------------------------------
Track::~Track()
{
    if (m_terrain)
        delete m_terrain;
}

// ----------------------------------------------------------------------------
void Track::create(u32 tx, u32 tz)
{
    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    m_terrain = new Terrain(sm->getRootSceneNode(), sm, 1, tx, tz, 2 * tx, 2 * tz);
}

// ----------------------------------------------------------------------------
void Track::load(path file)
{

}

// ----------------------------------------------------------------------------
void Track::save(path file)
{

}

// ----------------------------------------------------------------------------
void Track::quit()
{
    if (m_terrain)
        delete m_terrain;
}
