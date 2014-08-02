#include "mesh/driveline.hpp"

#include "editor.hpp"

#include <fstream>
#include <iostream>

// ----------------------------------------------------------------------------
// return value shows if the vertices are swaped or not
// current usage of this function doesn't support twisted drivelines, TODO!
bool DriveLine::order(vector3df* v1, vector3df* v2, vector3df* v3, vector3df* v4, vector3df n)
{
    vector3df e1, e2, p;
    e1 = *v2 - *v1;
    e2 = *v3 - *v1;
    p = e2.crossProduct(e1);
    if (p.dotProduct(n) > 0)
        return true;
    p = *v1; *v1 = *v2; *v2 = p;
    p = *v3; *v3 = *v4; *v4 = p;
    return false;
} // order

// ----------------------------------------------------------------------------
DriveLine::DriveLine(ISceneNode* parent, ISceneManager* mgr, s32 id, ISpline* s, 
                                                                       stringw n)
                                                    :IRoad(parent, mgr, id, s, n) 
{
    m_mesh.indices      = 0;
    m_mesh.vertices     = 0;
    m_mesh.vertex_count = 0;
    m_mesh.quad_count   = 0;

    m_material.Wireframe       = true;
    m_material.Lighting        = false;
    m_material.BackfaceCulling = false;
} // DriveLine

// ----------------------------------------------------------------------------
DriveLine::DriveLine(ISceneNode* parent, ISceneManager* mgr, s32 id, FILE* fp)
                                                    :IRoad(parent, mgr, id, fp) 
{
    m_mesh.indices      = 0;
    m_mesh.vertices     = 0;
    m_mesh.vertex_count = 0;
    m_mesh.quad_count   = 0;

    m_material.Wireframe       = true;
    m_material.Lighting        = false;
    m_material.BackfaceCulling = false;
} // DriveLine

// ----------------------------------------------------------------------------
void DriveLine::refresh()
{
    m_spline->updatePosition();

    if (m_mesh.vertices)
    {
        delete[] m_mesh.vertices;
        m_mesh.vertices = 0;
    }
    if (m_mesh.indices)
    {
        delete[] m_mesh.indices;
        m_mesh.indices = 0;
    }

    if (!m_spline->hasEnoughPoints()) return;

    int spn = m_spline->getPointNum() - 1;

    m_mesh.vertex_count = (int)(1.0f / m_detail * spn + 1) * 2;
    m_mesh.quad_count = (int)(1.0f / m_detail * spn);

    m_mesh.vertices = new S3DVertex2TCoords[m_mesh.vertex_count];
    m_mesh.indices = new u16[m_mesh.quad_count * 4];

    vector3df last_point = m_spline->p(0);
    vector3df point;
    vector3df n, v, w;

    float dt = m_detail / spn;

    point = m_spline->p(0);
    n = (m_spline->getNormal(dt) + m_spline->getNormal(0)) / 2.0f;
    v = m_spline->p(dt) - m_spline->p(0);
    w = v.crossProduct(n);

    n.normalize();
    w.normalize();

    w *= m_spline->getWidth(0);

    m_mesh.vertices[0].Pos = point - w * (m_width / 2.0f);
    m_mesh.vertices[0].Color = SColor(255, 255, 0, 0);
    m_mesh.vertices[1].Pos = point + w * (m_width / 2.0f);
    m_mesh.vertices[1].Color = SColor(255, 255, 0, 0);

    u32 j = 1;
    for (float t = dt; j <= m_mesh.quad_count; t += dt)
    {
        point = m_spline->p(t);
        n = (m_spline->getNormal(t) + m_spline->getNormal(t - dt)) / 2.0f;
        v = point - last_point;
        w = v.crossProduct(n);

        n.normalize();
        w.normalize();

        w *= m_spline->getWidth(t);

        m_mesh.vertices[2*j].Pos     = point - w * (m_width / 2.0f);
        m_mesh.vertices[2*j].Color   = SColor(255, 255, 0, 0);
        m_mesh.vertices[2*j+1].Pos   = point + w * (m_width / 2.0f);
        m_mesh.vertices[2*j+1].Color = SColor(255, 255, 0, 0);

        last_point = point;
        j++;
    }

    for (u32 i = 0; i < m_mesh.quad_count * 4; i++) m_mesh.indices[i] = i;

} // refresh

// ----------------------------------------------------------------------------
void DriveLine::build(path p)
{
    if (m_spline->getPointNum() < 2) return;
    std::ofstream quads;
    quads.open((p + "/quads.xml").c_str());
    quads << "<quads>\n";

    int spn = m_spline->getPointNum() - 1;
    vector3df last_point;
    vector3df point;
    vector3df n, v, w;
    vector3df v1, v2, v3, v4;
    float dt = m_detail / spn;

    // first two pointb
    point = m_spline->p(0);
    n = (m_spline->getNormal(dt) + m_spline->getNormal(0)) / 2.0f;
    v = m_spline->p(dt) - m_spline->p(0);
    w = v.crossProduct(n);
    n.normalize();
    w.normalize();
    w *= m_spline->getWidth(0);
    v1 = point - w * (m_width / 2.0f);
    v2 = point + w * (m_width / 2.0f);

    // second two point -> first quad
    point = m_spline->p(dt);
    n = (m_spline->getNormal(dt) + m_spline->getNormal(0)) / 2.0f;
    v = m_spline->p(dt) - m_spline->p(0);
    w = v.crossProduct(n);
    n.normalize();
    w.normalize();
    w *= m_spline->getWidth(0);
    v3 = point + w * (m_width / 2.0f);
    v4 = point - w * (m_width / 2.0f);
    order(&v1, &v2, &v3, &v4, m_spline->getNormal(0));

    quads << "  <quad  p0=\"" << v1.X << " " << v1.Y << " " << v1.Z << "\" ";
    quads << "p1=\"" << v2.X << " " << v2.Y << " " << v2.Z << "\" ";
    quads << "p2=\"" << v3.X << " " << v3.Y << " " << v3.Z;
    quads << "\" p3=\"" << v4.X << " " << v4.Y << " " << v4.Z << "\"/>\n";

    last_point = m_spline->p(dt);

    u32 j = 1;
    for (float t = dt * 2; j < m_mesh.quad_count; t += dt)
    {
        point = m_spline->p(t);
        n = (m_spline->getNormal(t) + m_spline->getNormal(t - dt)) / 2.0f;
        v = point - last_point;
        w = v.crossProduct(n);
        n.normalize();
        w.normalize();
        w *= m_spline->getWidth(t);
        v1 = point - w * m_width / 2.0f;
        v2 = point + w * m_width / 2.0f;
        order(&v1, &v2, &v3, &v4, m_spline->getNormal(t));
        quads << "  <quad  p0=\"" << j - 1 << ":3\" p1=\"" << j - 1 << ":2\" ";
        quads << "p2=\"" << v1.X << " " << v1.Y << " " << v1.Z;
        quads << "\" p3=\"" << v2.X << " " << v2.Y << " " << v2.Z << "\"/>\n";

        v3 = v1;
        v4 = v2;
        last_point = point;
        j++;
    }

    quads << "</quads>\n";

    std::ofstream  graph;
    graph.open((p + "/graph.xml").c_str());    

    graph << "<graph>\n";

    graph << "  <node-list from-quad=\"0\" to-quad=\"";
    graph << m_mesh.quad_count - 1 << "\"/>\n";

    graph << "  <edge-loop from=\"0\" to=\"" << m_mesh.quad_count - 1 << "\"/>\n";
    graph << "</graph>\n";

    graph.close();
}

// ----------------------------------------------------------------------------
void DriveLine::render()
{
    IVideoDriver* driver = SceneManager->getVideoDriver();

    if (m_mesh.vertices == 0) return;

    driver->setMaterial(m_material);
    driver->setTransform(ETS_WORLD, IdentityMatrix);

    driver->drawVertexPrimitiveList(&m_mesh.vertices[0], m_mesh.vertex_count,
        &m_mesh.indices[0], m_mesh.quad_count, video::EVT_2TCOORDS, EPT_QUAD_STRIP,
        video::EIT_16BIT);

} // render


