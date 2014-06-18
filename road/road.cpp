#include "road/road.hpp"

#include <iostream>
#include "assert.h"

// ----------------------------------------------------------------------------
void Road::calcVertexRow(vector3df p, vector3df n, vector3df w, int offset)
{
    
    for (int i = 0; i < m_width_vert_num / 2; i++)
    {
        m_mesh.vertices[offset + i].Pos = p + w * (i - m_width_vert_num / 4);

        m_mesh.vertices[offset + m_width_vert_num / 2 + i].Color = SColor(255, 0, 0, 255);
    }
    
    for (int i = 0; i < m_width_vert_num / 2; i++)
    {
        vector3df vec = m_mesh.vertices[offset + m_width_vert_num / 2 - i - 1].Pos - 0.3 * n;
        m_mesh.vertices[offset + m_width_vert_num / 2 + i].Pos = vec;
        m_mesh.vertices[offset + m_width_vert_num / 2 - i - 1].Color = SColor(255, 0, 255, 0);
    }
} // calcVertexRow

// ----------------------------------------------------------------------------
void Road::createIndexList(int nj, int ni)
{
    int ix = 0;
    for (int j = 0; j < nj - 1; j++)
    {
        for (int i = 0; i < ni - 1; i++)
        {
            m_mesh.indices[ix] = i + (j + 1)     * ni;   ix++;
            m_mesh.indices[ix] = i + 1 + (j + 1) * ni;   ix++;
            m_mesh.indices[ix] = i + 1 + j       * ni;   ix++;
                                                   
            m_mesh.indices[ix] = i + 1 + j       * ni;   ix++;
            m_mesh.indices[ix] = i + j           * ni;   ix++;
            m_mesh.indices[ix] = i + (j + 1)     * ni;   ix++;
        }
        /*
        m_mesh.indices[ix] = ni - 1 + (j + 1)     * ni;   ix++;
        m_mesh.indices[ix] = 0  + (j + 1)         * ni;   ix++;
        m_mesh.indices[ix] = 0  + j               * ni;   ix++;

        m_mesh.indices[ix] = 0 + j                * ni;   ix++;
        m_mesh.indices[ix] = ni - 1 + j           * ni;   ix++;
        m_mesh.indices[ix] = ni - 1 + (j + 1)     * ni;   ix++;
        */

    }
} // createIndexList

// ----------------------------------------------------------------------------
Road::Road(ISceneNode* parent, ISceneManager* mgr, s32 id, ISpline* s)
                              :ISceneNode(parent, mgr, id)
{
    m_spline            = s;
    m_mesh.indices      = 0;
    m_mesh.vertices     = 0;
    m_mesh.vertex_count = 0;
    m_mesh.quad_count   = 0;
    m_detail            = 0.02f;
    m_width             = 1.0f;
    m_width_vert_num    = 12;

    m_material.Wireframe       = true;
    m_material.Lighting        = false;
    m_material.BackfaceCulling = false;

} // Road


// ----------------------------------------------------------------------------
void Road::refresh()
{
    assert(m_width_vert_num % 4 == 0);

    if (!m_spline->hasEnoughPoints()) return;

    if (m_mesh.vertices) delete[] m_mesh.vertices;
    if (m_mesh.indices)  delete[] m_mesh.indices;

    m_mesh.vertex_count = (int)(1.0f / m_detail + 1) * m_width_vert_num;
    m_mesh.quad_count   = (int)(1.0f / m_detail)      * (m_width_vert_num-1);

    m_mesh.vertices = new S3DVertex[m_mesh.vertex_count];
    m_mesh.indices  = new u16[m_mesh.quad_count * 6];

    vector3df last_point = m_spline->p(0);
    vector3df point;
    vector3df n,v,w;


    point = m_spline->p(0);
    n = (m_spline->getNormal(m_detail) + m_spline->getNormal(0)) / 2.0f;
    v = m_spline->p(m_detail) - m_spline->p(0);
    w = v.crossProduct(n);

    calcVertexRow(point, n.normalize(), w.normalize(), 0);

    int j = 1;
    for (float t = m_detail; t <= 1.0; t += m_detail)
    {
        point  = m_spline->p(t);
        n = (m_spline->getNormal(t) + m_spline->getNormal(t-m_detail))/2.0f;
        v = point - last_point;
        w = v.crossProduct(n);

        calcVertexRow(point, n.normalize(), w.normalize(), j * m_width_vert_num);

        last_point = point;
        j++;
    }

    createIndexList((int)(1.0f / m_detail + 1), m_width_vert_num);

} // refresh

// ----------------------------------------------------------------------------
void Road::OnRegisterSceneNode()
{
    if (IsVisible)
        SceneManager->registerNodeForRendering(this);

    ISceneNode::OnRegisterSceneNode();

} // OnRegisterSceneNode

// ----------------------------------------------------------------------------
void Road::render()
{
    IVideoDriver* driver = SceneManager->getVideoDriver();

    if (m_mesh.vertices == 0) return;

    driver->setMaterial(m_material);
    driver->setTransform(ETS_WORLD, IdentityMatrix);
    
    driver->drawVertexPrimitiveList(&m_mesh.vertices[0], m_mesh.vertex_count,
        &m_mesh.indices[0], m_mesh.quad_count * 2,
        video::EVT_STANDARD, EPT_TRIANGLES,
        video::EIT_16BIT);

} // render

