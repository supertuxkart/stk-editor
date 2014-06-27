#include "commands/texmodcmd.hpp"

#include "entities/terrain.hpp"
#include "structs.hpp"

#include <assert.h>


// ----------------------------------------------------------------------------
TexModCmd::TexModCmd(Terrain* t) :ITCommand(t)
{
    m_tmp_address = new u8*[Terrain::SPIMG_X * Terrain::SPIMG_Y];
    m_tmp_values  = new s16[Terrain::SPIMG_X * Terrain::SPIMG_Y * 4];

    for (int j = 0; j < Terrain::SPIMG_Y; j++)
        for (int i = 0; i < Terrain::SPIMG_X; i++)
            for (int k = 0; k < 4; k++)
            m_tmp_values[4*j*Terrain::SPIMG_X + 4*i + k] = 0;

    for (int i = 0; i < Terrain::SPIMG_X; i++)
        for (int j = 0; j < Terrain::SPIMG_Y; j++)
            m_tmp_address[Terrain::SPIMG_X*j + i] = 0;

} // HeightModCmd

// ----------------------------------------------------------------------------
TexModCmd::~TexModCmd()
{
    if (m_tmp_values)   delete[] m_tmp_values;
    if (m_tmp_address)  delete[] m_tmp_address;
} // ~HeightModCmd

// ----------------------------------------------------------------------------
void TexModCmd::addVertex(TerrainChange* tc)
{
    assert(!m_finished);

    m_tmp_values[Terrain::SPIMG_X*tc->z * 4 + tc->x * 4]     += tc->db;
    m_tmp_values[Terrain::SPIMG_X*tc->z * 4 + tc->x * 4 + 1] += tc->dg;
    m_tmp_values[Terrain::SPIMG_X*tc->z * 4 + tc->x * 4 + 2] += tc->dr;
    m_tmp_values[Terrain::SPIMG_X*tc->z * 4 + tc->x * 4 + 3] += tc->da;

    m_tmp_address[Terrain::SPIMG_X*tc->z + tc->x] = 
        &tc->img[Terrain::SPIMG_X*tc->z*4  + tc->x *4];
}  // addVertex

// ----------------------------------------------------------------------------
void TexModCmd::finish()
{
    assert(!m_finished);

    ITexture* tex = m_terrain->getMaterial(0).getTexture(0);
    u8* img = (u8*)tex->lock(ETLM_READ_WRITE);

    for (int j = 0; j < Terrain::SPIMG_Y; j++)
        for (int i = 0; i < Terrain::SPIMG_X; i++)
            for (int k = 0; k < 4; k++)
            if (m_tmp_values[Terrain::SPIMG_X*j*4 + 4*i + k] != 0)
            {
                u8* u = m_tmp_address[Terrain::SPIMG_X*j + i] + k;
                s16 s = m_tmp_values[Terrain::SPIMG_X * j * 4 + i * 4 + k];
                m_list.push_back(std::pair<u8*, s16>(u,s));
            }

    delete[] m_tmp_values;
    m_tmp_values = 0;
    delete[] m_tmp_address;
    m_tmp_address = 0;

    tex->unlock();

    m_finished = true;
}  // finish


// ----------------------------------------------------------------------------
void TexModCmd::undo()
{
    ITexture* tex = m_terrain->getMaterial(0).getTexture(0);
    u8* img = (u8*)tex->lock(ETLM_READ_WRITE);

    std::list<std::pair<u8*, s16> >::iterator it;
    for (it = m_list.begin(); it != m_list.end(); it++)
    {
        s16 s = (s16)*(it->first);
        s -= it->second;
        *(it->first) = (u8)s;
    }

    tex->unlock();


} // undo

// ----------------------------------------------------------------------------
void TexModCmd::redo()
{
    ITexture* tex = m_terrain->getMaterial(0).getTexture(0);
    u8* img = (u8*)tex->lock(ETLM_READ_WRITE);

    std::list<std::pair<u8*, s16> >::iterator it;
    for (it = m_list.begin(); it != m_list.end(); it++)
    {
        s16 s = (s16)*(it->first);
        s += it->second;
        *(it->first) = (u8)s;
    }
    
    tex->unlock();


} // redo


