#include "entities/terrain.hpp"

Terrain::Terrain(ISceneNode* parent, ISceneManager* mgr, s32 id)
    :ISceneNode(parent, mgr, id)
{
    m_material.Wireframe = true;
    m_material.Lighting = false;
    ISceneNode::setAutomaticCulling(EAC_OFF);
    m_vertex_h_before = 0;
}


Terrain::~Terrain()
{
}


void Terrain::modify(line3d<float> ray, float radius, float dh, int MID)
{

    if (MID != m_last_mod_ID)
    {
        m_last_mod_ID = MID;
        if (m_vertex_h_before != 0) delete[] m_vertex_h_before;
        m_vertex_h_before = new float[m_vertex_count];
        for (int i = 0; i < m_vertex_count; i++)
        {
            m_vertex_h_before[i] = m_vertices[i].Pos.Y;
        }
    }

    // Terrain - ray intersection point
    vector3df p1 = ray.start - getPosition();
    vector3df p2 = ray.end   - getPosition();
    
    vector3df v  = (p2 - p1).normalize();
    float     t  = -p1.Y / v.Y;
    vector3df p = vector3df(p1.X + v.X * t, 0, p1.Z + v.Z * t);

    int ix = p.X / (m_x / m_nx);
    int iz = p.Z / (m_z / m_nz);

    int dx = (radius / (m_x / m_nx) + 1);
    int dz = (radius / (m_z / m_nz) + 1);
  
    bool b = true;

    vector2df kpos = vector2df(m_vertices[iz * m_nx + ix].Pos.X, 
                               m_vertices[iz * m_nx + ix].Pos.Z);

    for (int i = -dx; i <= dx; i++)
        for (int j = -dz; j <= dz; j++)
        {
            b = true;
            if (ix + i < 0 || ix + i > m_nx - 1) b = false;
            if (iz + j < 0 || iz + j > m_nz - 1) b = false;
            if (b)
            {
                vector2df pos = vector2df(m_vertices[(iz + j) * m_nx + ix + i].Pos.X,
                                          m_vertices[(iz + j) * m_nx + ix + i].Pos.Z);
                if ((kpos - pos).getLength() < radius) 
                {
                    float dist = radius - (kpos - pos).getLength();
                    float h = dist * dh / radius;
                    
                    m_vertices[(iz + j) * m_nx + ix + i].Pos.Y += h;
                    if (fabs((m_vertices[(iz + j) * m_nx + ix + i].Pos.Y -
                            m_vertex_h_before[(iz + j) * m_nx + ix + i])) > fabs(dh))
                    {
                        m_vertices[(iz + j) * m_nx + ix + i].Pos.Y =
                            m_vertex_h_before[(iz + j) * m_nx + ix + i] + dh;
                    }

                }
            }
        }

}

void Terrain::setSize(float x, float y, int nx, int nz)
{
    if (m_vertices) delete[] m_vertices;
    if (m_indices)  delete[] m_indices;

    m_vertex_count = nx * nz;
    m_vertices     = new S3DVertex[m_vertex_count];

    m_quad_count = (nx - 1) * (nz - 1);
    m_indices    = new u16[m_quad_count*6];
    
    for (int j = 0; j < nz; j++)
        for (int i = 0; i < nx; i++)
        {
            m_vertices[j * nx + i].Pos     = vector3df(x / nx * i, 0,  y / nz *j);
            m_vertices[j * nx + i].Color   = SColor(125, 255, 0, 0);
            m_vertices[j * nx + i].TCoords = vector2df(i / x, j / y);
        }

    int ix = 0;
    for (int j = 0; j < nz - 1; j++)
        for (int i = 0; i < nx - 1; i++)
        {
            m_indices[ix] = i +     (j + 1) * nx;   ix++;
            m_indices[ix] = i + 1 + (j + 1) * nx;   ix++;
            m_indices[ix] = i + 1 + j       * nx;   ix++;

            m_indices[ix] = i + 1 + j       * nx;   ix++;
            m_indices[ix] = i + j           * nx;   ix++;
            m_indices[ix] = i + (j + 1) * nx;   ix++;
        }
    
    m_x = x; m_nx = nx;
    m_z = y; m_nz = nz;

}

void Terrain::OnRegisterSceneNode()
{
    if (IsVisible)
        SceneManager->registerNodeForRendering(this);

    ISceneNode::OnRegisterSceneNode();
}

void Terrain::render()
{

    IVideoDriver* driver = SceneManager->getVideoDriver();

    driver->setMaterial(m_material);
    driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

    driver->drawVertexPrimitiveList(&m_vertices[0], m_vertex_count,
        &m_indices[0], m_quad_count * 2,
        video::EVT_STANDARD, EPT_TRIANGLES,
        video::EIT_16BIT);
}
