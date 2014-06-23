#include "road/driveline.hpp"


// ----------------------------------------------------------------------------
void DriveLine::refresh()
{

    int spn = m_spline->getPointNum() - 1;

    if (!m_spline->hasEnoughPoints()) return;

    if (m_mesh.vertices) delete[] m_mesh.vertices;
    if (m_mesh.indices)  delete[] m_mesh.indices;

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

    m_mesh.vertices[0].Pos = point - w * (m_width / 2.0);
    m_mesh.vertices[0].Color = SColor(255, 255, 0, 0);
    m_mesh.vertices[1].Pos = point + w * (m_width / 2.0);
    m_mesh.vertices[1].Color = SColor(255, 255, 0, 0);

    int j = 1;
    for (float t = dt; j <= m_mesh.quad_count; t += dt)
    {
        point = m_spline->p(t);
        n = (m_spline->getNormal(t) + m_spline->getNormal(t - dt)) / 2.0f;
        v = point - last_point;
        w = v.crossProduct(n);

        n.normalize();
        w.normalize();

        w *= m_spline->getWidth(t);

        m_mesh.vertices[2*j].Pos     = point - w * (m_width / 2.0);
        m_mesh.vertices[2*j].Color   = SColor(255, 255, 0, 0);
        m_mesh.vertices[2*j+1].Pos   = point + w * (m_width / 2.0);
        m_mesh.vertices[2*j+1].Color = SColor(255, 255, 0, 0);

        last_point = point;
        j++;
    }

    for (int i = 0; i < m_mesh.quad_count * 4; i++) m_mesh.indices[i] = i;

} // refresh

