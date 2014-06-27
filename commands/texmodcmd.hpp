#ifndef TEXMODCMD_HPP
#define TEXMODCMD_HPP

#include "commands/itcommand.hpp"

#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace core;
using namespace video;


class TexModCmd :public ITCommand
{
private:
    u32                     m_mod_count;
    u8*                     m_tmp_values;
    u8**                    m_tmp_address;
    bool*                   m_tmp_dirty;
    std::pair<u8*,u8>*      m_mod;
public:

    TexModCmd(Terrain* t);
    ~TexModCmd();

    void addVertex(TerrainChange* tc);
    void finish();

    void undo();
    void redo();
};


#endif

