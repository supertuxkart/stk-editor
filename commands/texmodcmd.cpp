#include "commands/texmodcmd.hpp"

#include "viewport/terrain.hpp"
#include "structs.hpp"

#include <assert.h>


// ----------------------------------------------------------------------------
TexModCmd::TexModCmd(Terrain* t) :ITCommand(t)
{
    m_mod_count   = 0;
    m_mod         = 0;
    m_tmp_address = new u32[Terrain::SPIMG_X * Terrain::SPIMG_Y];
    m_tmp_values  = new u8[Terrain::SPIMG_X * Terrain::SPIMG_Y * 4];
    m_tmp_dirty   = new bool[Terrain::SPIMG_X * Terrain::SPIMG_Y * 4];

    for (u32 j = 0; j < Terrain::SPIMG_Y; j++)
        for (u32 i = 0; i < Terrain::SPIMG_X; i++)
            for (u32 k = 0; k < 4; k++)
            {
                m_tmp_values[4 * j*Terrain::SPIMG_X + 4 * i + k] = 0;
                m_tmp_dirty[4 * j*Terrain::SPIMG_X + 4 * i + k]  = false;
            }
} // HeightModCmd

// ----------------------------------------------------------------------------
TexModCmd::~TexModCmd()
{
    if (m_tmp_values)   delete[] m_tmp_values;
    if (m_tmp_dirty)    delete[] m_tmp_dirty;
    if (m_tmp_address)  delete[] m_tmp_address;
    if (m_mod)          delete[] m_mod;
} // ~HeightModCmd

// ----------------------------------------------------------------------------
void TexModCmd::addVertex(TerrainChange* tc)
{
    assert(!m_finished);

    int ix = Terrain::SPIMG_X*tc->z * 4 + tc->x * 4;
    for (int k = 0; k < 4; k++)
        if (!m_tmp_dirty[ix + k] && tc->vchanged[k])
        {
            m_tmp_values[ix+k] = tc->old_v[k];
            m_tmp_dirty[ix+k]  = true;
            m_mod_count++;
        }

    m_tmp_address[Terrain::SPIMG_X*tc->z + tc->x] =
        &tc->img[Terrain::SPIMG_X*tc->z * 4 + tc->x * 4] - tc->img;
}  // addVertex

// ----------------------------------------------------------------------------
void TexModCmd::finish()
{
    assert(!m_finished);

    m_mod = new std::pair<u32, u8>[m_mod_count];
    u32 ix = 0;
    for (u32 j = 0; j < Terrain::SPIMG_Y; j++)
        for (u32 i = 0; i < Terrain::SPIMG_X; i++)
            for (u32 k = 0; k < 4; k++)
            if (m_tmp_dirty[Terrain::SPIMG_X*j*4 + 4*i + k])
            {
                u32 u = m_tmp_address[Terrain::SPIMG_X*j + i] + k * sizeof(u8);
                u8 s = m_tmp_values[Terrain::SPIMG_X * j * 4 + i * 4 + k];
                m_mod[ix] = std::pair<u32, u8>(u, s);
                ix++;
            }

    delete[] m_tmp_values;
    m_tmp_values = 0;
    delete[] m_tmp_address;
    m_tmp_address = 0;
    delete[] m_tmp_dirty;
    m_tmp_dirty = 0;

    m_finished = true;
}  // finish


// ----------------------------------------------------------------------------
void TexModCmd::undo()
{
    ITexture* tex = m_terrain->getMaterial(0).getTexture(0);
    u8* img;
    img = (u8*) tex->lock(ETLM_READ_WRITE);

    u8 tmp;
    for (u32 i = 0; i < m_mod_count; i++)
    {
        tmp = *((u8*)(m_mod[i].first + img));
        *((u8*)(m_mod[i].first + img)) = m_mod[i].second;
        m_mod[i].second = tmp;
    }

    tex->unlock();
} // undo

// ----------------------------------------------------------------------------
void TexModCmd::redo()
{
    undo();
} // redo


