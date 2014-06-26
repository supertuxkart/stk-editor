#include "commands/itcommand.hpp"

#include "entities/terrain.hpp"
#include "structs.hpp"

#include <assert.h>

// ----------------------------------------------------------------------------
// ITCommand-------------------------------------------------------------------
// ----------------------------------------------------------------------------
ITCommand::ITCommand(Terrain* t, u32 nx, u32 nz)
{
    m_finished = false;
    m_terrain  = t;

    m_nx = nx;
    m_nz = nz;

} // ITCommand

// ----------------------------------------------------------------------------
// HeightModCmd ---------------------------------------------------------------
// ----------------------------------------------------------------------------
HeightModCmd::HeightModCmd(Terrain* t, u32 nx, u32 nz) :ITCommand(t, nx, nz)
{
    m_tmp_value   = new float [nx*nz];

    for (int j = 0; j < nz; j++)
        for (int i = 0; i < nx; i++)
            m_tmp_value[j*nx + i] = 0;

    m_tmp_address = new float*[nx*nz];
} // HeightModCmd

// ----------------------------------------------------------------------------
HeightModCmd::~HeightModCmd()
{
    if (m_tmp_value)   delete[] m_tmp_value;
    if (m_tmp_address) delete[] m_tmp_address;
} // ~HeightModCmd

// ----------------------------------------------------------------------------
void HeightModCmd::addVertex(S3DVertex2TCoords &v, TerrainChange* tc)
{
    assert(!m_finished);
    m_tmp_value[m_nx*tc->z + tc->x]   += tc->dh;
    m_tmp_address[m_nx*tc->z + tc->x]  = &v.Pos.Y;
}  // addVertex

// ----------------------------------------------------------------------------
void HeightModCmd::finish()
{
    assert(!m_finished);

    for (int j = 0; j < m_nz; j++)
        for (int i = 0; i < m_nx; i++)
            if (m_tmp_value[m_nx*j + i] != 0)
            {
                m_list.push_back(std::pair<float*, float>(
                    m_tmp_address[m_nx*j + i], m_tmp_value[m_nx*j + i]));
            }

    delete[] m_tmp_value;
    m_tmp_value = 0;
    delete[] m_tmp_address;
    m_tmp_address = 0;

    m_finished = true;
}  // finish


// ----------------------------------------------------------------------------
void HeightModCmd::undo()
{
    std::list<std::pair<float*, float>>::iterator it;
    for (it = m_list.begin(); it != m_list.end(); it++)
        *(it->first) -= it->second;
    m_terrain->recalculateNormals();
} // undo

// ----------------------------------------------------------------------------
void HeightModCmd::redo()
{
    std::list<std::pair<float*, float>>::iterator it;
    for (it = m_list.begin(); it != m_list.end(); it++)
        *(it->first) += it->second;
    m_terrain->recalculateNormals();
} // redo
