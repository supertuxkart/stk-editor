#ifndef ITCOMMAND_HPP
#define ITCOMMAND_HPP

#include "commands/icommand.hpp"

#include <irrlicht.h>
#include <list>

using namespace irr;
using namespace scene;
using namespace core;
using namespace video;

class TerrainChange;
class Terrain;

// ----------------------------------------------------------------------------
class ITCommand :ICommand
{
protected:

    Terrain*    m_terrain;
    bool        m_finished;

public:
    ITCommand(Terrain* t) :m_finished(false), m_terrain(t){};
    virtual ~ITCommand() {};

    virtual void addVertex(TerrainChange* tm) = 0;
    virtual void finish() = 0;
}; // ITCommand

#endif
