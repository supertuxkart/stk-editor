#include "entities/terrain.hpp"

// ----------------------------------------------------------------------------
/** This function calculates the index of the vertex, which is closest to the
 *  ray - (y=0) plain intersection point, and also give back some additional 
 *  data to describe a circle around the point
 *  \param ray A 3d line which hits the surface
 *  \param r Radius of the circle
 *  \param cpos Coordinates of the vertex, which is closest to the intersection
 *  \param ix index of the vertex closest to the intersection
 *  \param iz 
 *  \param dx [ix+-dx][iz+-dz] describes a square, which contain every point p,
 *  with | cpos - p | < r
 *  \param dz
 */
void Terrain::coinAroundIntersection(line3d<float> ray, float r, vector2df* cpos,
                                     int* ix, int* iz, int* dx, int* dz)
{
    vector3df p1 = ray.start - getPosition();
    vector3df p2 = ray.end - getPosition();

    vector3df v = (p2 - p1).normalize();
    float     t = -p1.Y / v.Y;
    vector3df p = vector3df(p1.X + v.X * t, 0, p1.Z + v.Z * t);

    int iix = p.X / (m_x / m_nx);
    int iiz = p.Z / (m_z / m_nz);

    *dx = (int) (r / (m_x / m_nx) + 1);
    *dz = (int) (r / (m_z / m_nz) + 1);

    bool b = true;

    *cpos = vector2df(m_vertices[iiz * m_nx + iix].Pos.X,
        m_vertices[iiz * m_nx + iix].Pos.Z);
    *ix = iix;
    *iz = iiz;
}


// ----------------------------------------------------------------------------
/** The constuctor does not create a usable surface, it just call the base class
 *  and set some default value. setSize() function creates the vertices.
*/
Terrain::Terrain(ISceneNode* parent, ISceneManager* mgr, s32 id)
    :ISceneNode(parent, mgr, id)
{
    m_vertices = 0;
    m_indices  = 0;
    m_material.Wireframe = true;
    m_material.Lighting = false;
    ISceneNode::setAutomaticCulling(EAC_OFF);
    m_vertex_h_before    = 0;
    m_last_mod_ID        = -1;

} // Terrain

// ----------------------------------------------------------------------------
Terrain::~Terrain()
{
    if (m_vertices) delete[] m_vertices;
    if (m_indices)  delete[] m_indices;
} // ~Terrain



// ----------------------------------------------------------------------------
/** Function for the basic terrain modifier tool: it is used to change terrain
 *  height around a point. In every phase, the difference between old and new 
 *  height is limited to tm.dh. A phase is defined by an ID: it is stored in
 *  m_last_mod_ID - one phase can consist of more than one function call
 *  \param ray A 3d line which determines the central point
 *  \param tm It contains data about the modification, like radius, intensity, ID
 */
void Terrain::modify(line3d<float> ray, TerrainMod tm)
{

    if (tm.ID != m_last_mod_ID)
    {
        m_last_mod_ID = tm.ID;
        if (m_vertex_h_before != 0) delete[] m_vertex_h_before;
        m_vertex_h_before = new float[m_vertex_count];
        for (unsigned int i = 0; i < m_vertex_count; i++)
        {
            m_vertex_h_before[i] = m_vertices[i].Pos.Y;
        }
    }

    int ix, iz, dx, dz;  
    vector2df cpos;
    coinAroundIntersection(ray, tm.radius, &cpos, &ix, &iz, &dx, &dz);

    bool b = true;

    for (int i = -dx; i <= dx; i++)
        for (int j = -dz; j <= dz; j++)
        {
            b = true;
            // check if the point is outside of the terrain
            if (ix + i < 0 || ix + i > m_nx - 1) b = false;
            if (iz + j < 0 || iz + j > m_nz - 1) b = false;
            if (b)
            {
                vector2df pos = vector2df(m_vertices[(iz + j) * m_nx + ix + i].Pos.X,
                                          m_vertices[(iz + j) * m_nx + ix + i].Pos.Z);
                if ((cpos - pos).getLength() < tm.radius) 
                {
                    float h;
                    // edge type and the distance from the intersection point will define
                    // the intensity
                    if (tm.edge_type == 1) h = tm.dh;
                    else
                    {
                        float dist = tm.radius - (cpos - pos).getLength();
                        h = dist * tm.dh / tm.radius;
                        if (tm.edge_type == 3) h = sqrt(fabs(h)) * h / fabs(h);
                    }
                    // new height
                    m_vertices[(iz + j) * m_nx + ix + i].Pos.Y += h;
                    
                    // check if the limit is reached, correction if necessary
                    if (fabs((m_vertices[(iz + j) * m_nx + ix + i].Pos.Y -
                            m_vertex_h_before[(iz + j) * m_nx + ix + i])) > fabs(tm.dh))
                    {
                        m_vertices[(iz + j) * m_nx + ix + i].Pos.Y =
                            m_vertex_h_before[(iz + j) * m_nx + ix + i] + tm.dh;
                    }
                } // distance < radius
            } // b -> this squere point is a valid vertex
        } // for loop - critical squere
} // modify

// ----------------------------------------------------------------------------
/** This function will create the vertices and the indicies. It is possible to
 *  call it anytime to resize the terrain, but doing so means the death of the
 *  old terrain, as new vertices are created with height 0
 *  \param x the size of the terrain along the x axis
 *  \param z the size of the terrain along the y axis
 *  \param nx vertex count along the x axis
 *  \param nz vertex count along the z axis
 */
void Terrain::setSize(float x, float z, int nx, int nz)
{
    m_bounding_box = aabbox3d<f32>(0,0,0,x,0,z);

    if (m_vertices) delete[] m_vertices;
    if (m_indices)  delete[] m_indices;

    m_vertex_count = nx * nz;
    m_vertices     = new S3DVertex[m_vertex_count];

    m_quad_count = (nx - 1) * (nz - 1);
    m_indices    = new u16[m_quad_count*6];
    
    for (int j = 0; j < nz; j++)
        for (int i = 0; i < nx; i++)
        {
            m_vertices[j * nx + i].Pos     = vector3df(x / nx * i, 0,  z / nz *j);
            m_vertices[j * nx + i].Color   = SColor(125, 0, 255, 0);
            m_vertices[j * nx + i].TCoords = vector2df(i / x, j / z);
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
    m_z = z; m_nz = nz;

} // setSize

// ----------------------------------------------------------------------------
void Terrain::OnRegisterSceneNode()
{
    if (IsVisible)
        SceneManager->registerNodeForRendering(this);

    ISceneNode::OnRegisterSceneNode();
} // OnRegisterSceneNode

// ----------------------------------------------------------------------------
void Terrain::render()
{

    IVideoDriver* driver = SceneManager->getVideoDriver();

    driver->setMaterial(m_material);
    driver->setTransform(video::ETS_WORLD, IdentityMatrix);

    driver->drawVertexPrimitiveList(&m_vertices[0], m_vertex_count,
        &m_indices[0], m_quad_count * 2,
        video::EVT_STANDARD, EPT_TRIANGLES,
        video::EIT_16BIT);

} // render
