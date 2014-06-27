#ifndef TEXMODCMD_HPP
#define TEXMODCMD_HPP

#include "commands/itcommand.hpp"

#include <irrlicht.h>
#include <list>

using namespace irr;
using namespace scene;
using namespace core;
using namespace video;


class TexModCmd :public ITCommand
{
private:
    s16*    m_tmp_values;
    u8**    m_tmp_address;
    std::list<std::pair<u8*,s16> > m_list;
public:

    TexModCmd(Terrain* t);
    ~TexModCmd();

    void addVertex(TerrainChange* tc);
    void finish();

    void undo();
    void redo();
};


#endif

