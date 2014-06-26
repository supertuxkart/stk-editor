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
    u32         m_nx, m_nz;
    bool        m_finished;

public:
    ITCommand(Terrain* t, u32 nx, u32 nz);
    virtual ~ITCommand() {};

    virtual void addVertex(S3DVertex2TCoords &v, TerrainChange* tm) = 0;
    virtual void finish() = 0;
}; // ITCommand


// ----------------------------------------------------------------------------
class HeightModCmd :ITCommand
{
private:
    float* m_tmp_value;
    float** m_tmp_address;
    std::list<std::pair<float*, float> > m_list;
public:

    HeightModCmd(Terrain* t, u32 nx, u32 nz);
    ~HeightModCmd();

    void addVertex(S3DVertex2TCoords &v, TerrainChange* tc);
    void finish();

    void undo();
    void redo();
};


#endif

