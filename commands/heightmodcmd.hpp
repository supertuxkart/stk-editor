#ifndef HEIGHTMODCMD_HPP
#define HEIGHTMODCMD_HPP

#include "commands/itcommand.hpp"

#include <irrlicht.h>
#include <list>

using namespace irr;
using namespace scene;
using namespace core;
using namespace video;

// ----------------------------------------------------------------------------
class HeightModCmd :public ITCommand
{
private:

    u32     m_nx, m_nz;
    float*  m_tmp_value;
    float** m_tmp_address;
    std::list<std::pair<float*, float> > m_list;
public:

    HeightModCmd(Terrain* t, u32 nx, u32 nz);
    ~HeightModCmd();

    void addVertex(TerrainChange* tc);
    void finish();

    void undo();
    void redo();
};

#endif
