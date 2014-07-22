#include "mesh/terrain.hpp"

#include "editor.hpp"

#include "mesh/shader.hpp"

#include "commands/iterrain_cmd.hpp"

#include <algorithm>
#include <assert.h>

const u32 Terrain::SPIMG_X = 1024;
const u32 Terrain::SPIMG_Y = 1024;

// ----------------------------------------------------------------------------
/**  Always set m_fp before calling this function!!!
 *   callOnVertices will use data from tmod to find the vertices described by
 *   tmod.ray and tmod.radius, and call m_fp on them.
 *  \param tmod data about the modification
 *  \param call_outside true means that m_fp will be called on non-exsisting
    vertices as well. tmod.in_terrain flag will distinguish them
 *  \param call_in_square true means that m_fp is called if the distance is < r
 *  along (x || z) axis
 */
void Terrain::callOnVertices(TerrainMod* tmod, bool call_outside,
    bool call_in_square)
{

    int ix, iz, dx, dz;
    vector2df cpos;

    if (!intersectionPoint(tmod->ray, tmod->radius, &cpos, &ix, &iz))
        return;

    dx = (int)(tmod->radius / (m_x / m_nx) + 1);
    dz = (int)(tmod->radius / (m_z / m_nz) + 1);

    tmod->index = 0;
    for (int j = -dz; j <= dz; j++)
    for (int i = -dx; i <= dx; i++)
    {
        tmod->in_terrain = true;
        // check if the point is outside of the terrain
        if (ix + i < 1 || ix + i > m_nx - 2 || iz + j < 1 || iz + j > m_nz - 2)
            tmod->in_terrain = false;

        if (call_outside && !tmod->in_terrain) (*this.*m_fp_h)(*tmod, ix, iz, i, j);
        else if (tmod->in_terrain)
        {
            S3DVertex2TCoords vert = m_mesh.vertices[(iz + j) * m_nx + ix + i];
            vector2df pos;
            pos = vector2df(vert.Pos.X, vert.Pos.Z);

            // check if the point is in radius
            tmod->dist = tmod->radius - (cpos - pos).getLength();

            if (tmod->dist > 0 || call_in_square)
            {
                (*this.*m_fp_h)(*tmod, ix, iz, i, j);
            }
        }
        tmod->index += 1;
    } // for loop - square
} // callOnVertices


// ----------------------------------------------------------------------------
void Terrain::vertexHighlight(const TerrainMod& tm, int ix, int iz, int i, int j)
{
    if (tm.in_terrain)
    {
        m_highlight_mesh.vertices[tm.index] =
            m_mesh.vertices[(iz + j) * m_nx + ix + i];
        m_highlight_mesh.vertices[tm.index].Pos.Y += 0.2f;
        m_highlight_mesh.vertices[tm.index].Color = SColor(255, 255, 0, 0);

        if (tm.dist < 0)
        {
            // not in radius
            m_highlight_mesh.vertices[tm.index].Color = SColor(0, 255, 0, 0);
        }
    } // tm.in_terrain
    else
    {
        m_highlight_mesh.vertices[tm.index].Pos =
            vector3df(m_x / m_nx * (ix + i), 0, m_z / m_nz *(iz + j));

        m_highlight_mesh.vertices[tm.index].Color = SColor(0, 255, 0, 0);
    }
} // vertexHighlight

// ----------------------------------------------------------------------------
void Terrain::vertexHeight(const TerrainMod& tm, int ix, int iz, int i, int j)
{
    float old_value;
    float h;
    // edge type and the distance from the intersection point will define
    // the intensity
    if (tm.edge_type == 1) h = tm.dh;
    else
    {
        h = tm.dist * tm.dh / tm.radius;
        if (tm.edge_type == 3) h = sqrt(fabs(h)) * h / fabs(h);
    }
    // new height
    f32* y = &(m_mesh.vertices[(iz + j) * m_nx + ix + i].Pos.Y);
    old_value = *y;
    *y += h;

    // check if max / min value is ok
                    {
                        if (tm.max && *y > tm.max_v) *y = tm.max_v;
                        if (tm.min && *y < tm.min_v) *y = tm.min_v;
                    }

    TerrainChange tc;
    tc.x  = ix+i;
    tc.z  = iz+j;
    tc.dh = *y - old_value;
    tc.v  = &m_mesh.vertices[(iz + j) * m_nx + ix + i];
    tm.cmd->addVertex(&tc);
} // vertexHeight


// ----------------------------------------------------------------------------
/** This function calculates the indices of the vertex, which is closest to the
 *  ray - (y=0) plain intersection point
 *  \param ray A 3d line which hits the surface
 *  \param r Radius of the circle
 *  \param cpos Coordinates of the vertex, which is closest to the intersection
 *  \param ix index of the vertex closest to the intersection
 *  \param iz
 */
bool Terrain::intersectionPoint(const line3df& ray, float r,
                                vector2df* cpos, int* ix, int* iz)
{
    vector3df p1 = ray.start - getPosition();
    vector3df p2 = ray.end - getPosition();

    vector3df v = (p2 - p1).normalize();
    float     t = -p1.Y / v.Y;
    vector3df p = vector3df(p1.X + v.X * t, 0, p1.Z + v.Z * t);

    s32 iix = (int) (p.X / (m_x / m_nx) + 0.5);
    s32 iiz = (int) (p.Z / (m_z / m_nz) + 0.5);

    if (iix < 0 || iiz < 0 || iix >= m_nx || iiz >= m_nz)
    {
        *cpos = vector2df(p.X,p.Z);
        return false;
    }

    *cpos = vector2df(m_mesh.vertices[iiz * m_nx + iix].Pos.X,
                      m_mesh.vertices[iiz * m_nx + iix].Pos.Z);
    *ix = iix;
    *iz = iiz;

    return true;
} // intersectionPoint

// ----------------------------------------------------------------------------
void Terrain::createIndexList(u16* indices, int x, int z)
{
    int ix = 0;
    for (int j = 0; j < z - 1; j++)
        for (int i = 0; i < x - 1; i++)
        {
            indices[ix] = i +     (j + 1) * x;   ix++;
            indices[ix] = i + 1 + (j + 1) * x;   ix++;
            indices[ix] = i + 1 +  j      * x;   ix++;

            indices[ix] = i + 1 +  j      * x;   ix++;
            indices[ix] = i +  j          * x;   ix++;
            indices[ix] = i + (j + 1)     * x;   ix++;
        }
} // createIndexList

// ----------------------------------------------------------------------------
void Terrain::recalculateNormals()
{
    vector3df a, b, c, d;
    vector3df cp;
    for (u32 j = 1; j < m_nz - 1; j++)
        for (u32 i = 1; i < m_nx - 1; i++)
        {
            cp = m_mesh.vertices[j * m_nx + i].Pos;
            a = m_mesh.vertices[(j - 1) * m_nx + i].Pos - cp;
            b = m_mesh.vertices[j * m_nx + i + 1].Pos - cp;
            c = m_mesh.vertices[(j + 1) * m_nx + i].Pos - cp;
            d = m_mesh.vertices[j * m_nx + i - 1].Pos - cp;

            m_mesh.vertices[j * m_nx + i].Normal =
                    ((a.crossProduct(d) + d.crossProduct(c) +
                    c.crossProduct(b) + b.crossProduct(a)) / 4.0).normalize();

        }
} // recalculateNormals

// ----------------------------------------------------------------------------
void Terrain::refreshBB()
{
    f32 max = -10000;
    f32 min =  10000;
    float tmp;
    for (u32 i = 0; i < m_mesh.vertex_count; i++)
    {
        tmp = m_mesh.vertices[i].Pos.Y;
        if (tmp > max) max = tmp;
        if (tmp < min) min = tmp;
    }
    m_bounding_box = aabbox3d<f32>(0, min, 0, m_x, max, m_z);
}

// ----------------------------------------------------------------------------
void Terrain::initMaterials()
{
    IVideoDriver* vd = Editor::getEditor()->getVideoDriver();

    IGPUProgrammingServices* gpu = vd->getGPUProgrammingServices();
    s32 material_type;
    material_type = gpu->addHighLevelShaderMaterial(
        vs, ps, this, EMT_SOLID);

    m_material.MaterialType = (E_MATERIAL_TYPE)material_type;

    m_material.setTexture(1, createSplattingImg());
    m_material.setTexture(2, vd->getTexture(L"dirt.jpg"));
    m_material.setTexture(3, vd->getTexture(L"grass2.jpg"));
    m_material.setTexture(4, vd->getTexture(L"rock_brown.jpg"));
    m_material.setTexture(5, vd->getTexture(L"sand2.jpg"));
    m_material.setTexture(0, m_material.getTexture(1));

    m_highlight_material.Lighting = false;
    m_highlight_material.MaterialType = EMT_TRANSPARENT_VERTEX_ALPHA;
    m_highlight_material.Wireframe = true;
}

// ----------------------------------------------------------------------------
ITexture* Terrain::createSplattingImg()
{
    IVideoDriver* vd = Editor::getEditor()->getVideoDriver();

    IImage* img = vd->createImage(ECF_A8R8G8B8, dimension2du(SPIMG_X, SPIMG_Y));

    for (u32 i = 0; i < SPIMG_X; i++)
        for (u32 j = 0; j < SPIMG_Y; j++)
            img->setPixel(i, j, SColor(255, 0, 0, 0));

    m_splattingImg = img;
    return vd->addTexture("splatt.png", img);
} // initSplattingImg

// ----------------------------------------------------------------------------
void Terrain::draw(const TerrainMod& tm)
{

    vector3df p1 = tm.ray.start - getPosition();
    vector3df p2 = tm.ray.end   - getPosition();

    vector3df v = (p2 - p1).normalize();
    float     t = -p1.Y / v.Y;
    vector3df p = vector3df(p1.X + v.X * t, 0, p1.Z + v.Z * t);

    u32 ix = (int)(p.X / (m_x / m_nx) + 0.5);
    u32 iz = (int)(p.Z / (m_z / m_nz) + 0.5);

    if (ix < 0 || iz < 0 || ix >= m_nx || iz >= m_nz) return;

    u8* img;
    img = (u8*) m_material.getTexture(1)->lock(ETLM_READ_WRITE);

    vector2df tex = m_mesh.vertices[iz * m_nx + ix].TCoords2;

    vector2di tc = vector2di((int)(tex.X * SPIMG_X), (int)(tex.Y * SPIMG_X));

    int r = (int)(tm.radius / m_x * SPIMG_X);

    double e;

    for (int i = tc.X - r; i < tc.X + r; i++)
        for (int j = tc.Y - r; j < tc.Y + r; j++)
        {
        if ((i - tc.X)*(i - tc.X) + (j - tc.Y)*(j - tc.Y) < r*r)
            if ((i > 0 && i < (s32)SPIMG_X && j > 0 && j < (s32)SPIMG_Y))
            {
            TerrainChange terr_ch;
            terr_ch.x = i;
            terr_ch.z = j;
            
            e = tm.dh / 5.0f;
            if (!tm.left_click) e *= -1;

            if (tm.edge_type > 1)
            {
                e *= (1 - ((i - tc.X)*(i - tc.X) + (j - tc.Y)*(j - tc.Y)) / (f32)(r*r));
                if (tm.edge_type == 3) e = sqrt(fabs(e)) * e / fabs(e);
            }
            
            for (int k = 0; k < 4; k++)
                terr_ch.old_v[k] = img[j * SPIMG_X * 4 + i * 4 + k];

            switch (tm.type)
                {
                case HARD_BRUSH:
                    pixelHardBrush(img, j * SPIMG_X * 4 + i * 4, tm.col_mask, !tm.left_click);
                    break;
                case SOFT_BRUSH:
                    pixelSoftBrush(img, j * SPIMG_X * 4 + i * 4, tm.col_mask, e);
                    break;
                case BRIGHTNESS_MOD:
                    pixelBrigBrush(img, j * SPIMG_X * 4 + i * 4, e * 255);
                    break;
                }

            for (int k = 0; k < 4; k++)
                terr_ch.vchanged[k] = (terr_ch.old_v[k] != img[j * SPIMG_X * 4 + i * 4 + k]);

            terr_ch.img = img;
            tm.cmd->addVertex(&terr_ch);
            }

        }
    m_material.getTexture(1)->unlock();

} // draw


// ----------------------------------------------------------------------------
void Terrain::pixelHardBrush(u8* img, int ix, SColor col, bool erase)
{
    if (erase)
    {
        if (col.getBlue() > 0)  { img[ix]     = 0; return; }
        if (col.getGreen() > 0) { img[ix + 1] = 0; return; }
        if (col.getRed() > 0)   { img[ix + 2] = 0; return; }
        return;
    }

    img[ix + 2] = col.getRed();
    img[ix + 1] = col.getGreen();
    img[ix + 0] = col.getBlue();

} // vertexHardBrush

// ----------------------------------------------------------------------------
void Terrain::pixelSoftBrush(u8* img, int ix, SColor col, double e)
{
    if (e > 0)
    {
        img[ix + 3] = (u8) std::max(std::min(img[ix + 3] + e * col.getAlpha()
            - e * col.getRed() - e * col.getGreen() - e * col.getBlue(), 255.0), 0.0);
        img[ix + 2] = (u8) std::max(std::min(img[ix + 2] + e * col.getRed()
            - e * col.getAlpha() - e * col.getGreen() - e * col.getBlue(), 255.0), 0.0);
        img[ix + 1] = (u8) std::max(std::min(img[ix + 1] + e * col.getGreen()
            - e * col.getRed() - e * col.getAlpha() - e * col.getBlue(), 255.0), 0.0);
        img[ix + 0] = (u8) std::max(std::min(img[ix + 0] + e * col.getBlue()
            - e * col.getRed() - e * col.getGreen() - e * col.getAlpha(), 255.0), 0.0);
    }
    else
    {
        img[ix + 3] = (u8) std::max(img[ix + 3] + e * col.getAlpha(), 0.0);
        img[ix + 2] = (u8) std::max(img[ix + 2] + e * col.getRed(), 0.0);
        img[ix + 1] = (u8) std::max(img[ix + 1] + e * col.getGreen(), 0.0);
        img[ix + 0] = (u8) std::max(img[ix + 0] + e * col.getBlue(), 0.0);
    }

} // vertexSoftBrush

// ----------------------------------------------------------------------------
void Terrain::pixelBrigBrush(u8* img, int ix, double e)
{
    for (int i = 0; i < 4; i++)
    if (img[ix + i] > 0)
        img[ix + i] = (u8) std::max(std::min(img[ix + i] + e, 255.0), 1.0);
} // vertexBrigBrush

// ----------------------------------------------------------------------------
Terrain::Terrain(ISceneNode* parent, ISceneManager* mgr, s32 id,
                 float x, float z, u32 nx, u32 nz)
    :ISceneNode(parent, mgr, id), m_nx(nx), m_nz(nz)
{

    m_bounding_box = aabbox3d<f32>(0, 0, 0, x, 0, z);

    setAutomaticCulling(EAC_OFF);

    m_tile_num_x = 10;
    m_tile_num_z = 10;

    m_mesh.vertex_count = nx * nz;
    m_mesh.vertices = new S3DVertex2TCoords[m_mesh.vertex_count];

    m_mesh.quad_count = (nx - 1) * (nz - 1);
    m_mesh.indices = new u16[m_mesh.quad_count * 6];

    for (u32 j = 0; j < nz; j++)
    for (u32 i = 0; i < nx; i++)
    {
        m_mesh.vertices[j * nx + i].Pos = vector3df(x / nx * i, 0, z / nz *j);
        m_mesh.vertices[j * nx + i].Color = SColor(255, 255, 255, 255);
        m_mesh.vertices[j * nx + i].TCoords  =
            vector2df(i / (float)nx * m_tile_num_x, j / (float)nz * m_tile_num_z);
        m_mesh.vertices[j * nx + i].TCoords2 = vector2df(i / (float)nx, j / (float)nz);
    }

    createIndexList(m_mesh.indices, nx, nz);

    m_x = x;
    m_z = z;

    recalculateNormals();

    m_highlight_mesh.vertices = 0;
    m_highlight_mesh.indices = 0;

    initMaterials();

    m_highlight_visible = false;

} // Terrain

// ----------------------------------------------------------------------------
Terrain::Terrain(ISceneNode* parent, ISceneManager* mgr, s32 id, FILE* fp)
                                              :ISceneNode(parent, mgr, id)
{
    setAutomaticCulling(EAC_OFF);

    fread(&m_x, sizeof(f32), 1, fp);
    fread(&m_z, sizeof(f32), 1, fp);

    fread(&m_nx, sizeof(u32), 1, fp);
    fread(&m_nz, sizeof(u32), 1, fp);

    m_bounding_box = aabbox3d<f32>(0, 0, 0, m_x, 0, m_z);

    m_tile_num_x = 10;
    m_tile_num_z = 10;

    m_mesh.vertex_count = m_nx * m_nz;
    m_mesh.vertices = new S3DVertex2TCoords[m_mesh.vertex_count];

    m_mesh.quad_count = (m_nx - 1) * (m_nz - 1);
    m_mesh.indices = new u16[m_mesh.quad_count * 6];

    for (u32 j = 0; j < m_nz; j++)
    for (u32 i = 0; i < m_nx; i++)
    {
        m_mesh.vertices[j * m_nx + i].Pos =
            vector3df(m_x / m_nx * i, 0, m_z / m_nz *j);

        fread(&m_mesh.vertices[j * m_nx + i].Pos.Y, sizeof(f32), 1, fp);

        m_mesh.vertices[j * m_nx + i].Color = SColor(255, 255, 255, 255);

        m_mesh.vertices[j * m_nx + i].TCoords =
            vector2df(i / (float)m_nx * m_tile_num_x, j / (float)m_nz * m_tile_num_z);

        m_mesh.vertices[j * m_nx + i].TCoords2 =
            vector2df(i / (float)m_nx, j / (float)m_nz);
    }

    createIndexList(m_mesh.indices, m_nx, m_nz);

    recalculateNormals();

    m_highlight_mesh.vertices = 0;
    m_highlight_mesh.indices = 0;

    initMaterials();

    u32 x, y;
    fread(&x, sizeof(u32), 1, fp);
    fread(&y, sizeof(u32), 1, fp);

    assert(x == SPIMG_X && y == SPIMG_Y);

    fread(m_material.getTexture(1)->lock(ETLM_WRITE_ONLY), sizeof(u8),
                                             4 * SPIMG_X*SPIMG_Y, fp);
    m_material.getTexture(1)->unlock();

    m_highlight_visible = false;

} // Terrain - fp


// ----------------------------------------------------------------------------
Terrain::~Terrain()
{
    if (m_mesh.vertices) delete[] m_mesh.vertices;
    if (m_mesh.indices)  delete[] m_mesh.indices;

    if (m_highlight_mesh.vertices) delete[] m_highlight_mesh.vertices;
    if (m_highlight_mesh.indices)  delete[] m_highlight_mesh.indices;

} // ~Terrain


// ----------------------------------------------------------------------------
/** Function for the basic terrain modifier tool: it is used to change terrain
 *  height around a point.
 *  \param tm It contains data about the modification, like radius, intensity
 */
void Terrain::modify(TerrainMod* tm)
{
    if (tm->type == HEIGHT_MOD)
    {
        if (!tm->left_click) tm->dh *= -1;

        m_fp_h = &Terrain::vertexHeight;
        callOnVertices(tm);
        if (!tm->left_click) tm->dh *= -1;
        recalculateNormals();
        highlight(tm);
        refreshBB();
        return;
    }
    else
        draw(*tm);
} // modify

// ----------------------------------------------------------------------------
void Terrain::highlight(TerrainMod* tm)
{

    if (m_highlight_mesh.vertices)
    {
        delete[] m_highlight_mesh.vertices;
        m_highlight_mesh.vertices = 0;
    }
    if (m_highlight_mesh.indices)
    {
        delete[] m_highlight_mesh.indices;
        m_highlight_mesh.indices = 0;
    }

    int dx, dz;
    dx = (int)(tm->radius / (m_x / m_nx) + 1);
    dz = (int)(tm->radius / (m_z / m_nz) + 1);

    int x = 2 * dx + 1;
    int z = 2 * dz + 1;
    m_highlight_mesh.vertex_count = x * z;

    m_highlight_mesh.vertices = new S3DVertex2TCoords[m_highlight_mesh.vertex_count];
    m_highlight_mesh.quad_count = (x - 1) * (z - 1);
    m_highlight_mesh.indices = new u16[m_highlight_mesh.quad_count * 6];

    m_fp_h = &Terrain::vertexHighlight;
    callOnVertices(tm,true,true);

    createIndexList(m_highlight_mesh.indices, x, z);

} // highlight


// ----------------------------------------------------------------------------
vector3df Terrain::placeBBtoGround(const aabbox3d<f32>& box, line3d<float> ray)
{
    int ix, iz, dx, dz;
    vector2df cpos;
    if (!intersectionPoint(ray, 0, &cpos, &ix, &iz))
        return vector3df(cpos.X,0,cpos.Y);

    f32 max_h = -30000;
    dx = (int)(box.MaxEdge.X / (m_x / m_nx) + 1);
    dz = (int)(box.MaxEdge.Z / (m_z / m_nz) + 1);

    for (int i = -dx; i <= dx; i++)
        for (int j = -dz; j <= dz; j++)
        {
            // check if the point is outside of the terrain
            if (ix + i > 0 && ix + i < m_nx && iz + j > 0 && iz + j < m_nz)
            {
                max_h = std::max((m_mesh.vertices[(iz + j) * m_nx + ix + i].Pos.Y),max_h);
            } // this square point is a valid vertex
        } // for loop - critical rectangle

    return vector3df(cpos.X, max_h - box.MinEdge.Y, cpos.Y);

} // placeBBtoGround

// ----------------------------------------------------------------------------
CMeshBuffer<S3DVertex2TCoords>* Terrain::build(path p)
{
    IrrlichtDevice* device = Editor::getEditor()->getDevice();

    CMeshBuffer<S3DVertex2TCoords>* mb = new CMeshBuffer<S3DVertex2TCoords>();

    mb->Vertices.reallocate(m_mesh.vertex_count);
    mb->Indices.reallocate(m_mesh.quad_count * 6);

    for (u32 i = 0; i < m_mesh.vertex_count; i++)
        mb->Vertices.push_back(m_mesh.vertices[i]);
    for (u32 i = 0; i < m_mesh.quad_count * 6; i++)
        mb->Indices.push_back(m_mesh.indices[i]);

    mb->recalculateBoundingBox();
    mb->Material = m_material;

    ITexture* texture = m_material.getTexture(1);
    IImage* image = device->getVideoDriver()->createImage(texture, position2di(0, 0),
        texture->getSize());
    stringc name = p + "/splatt.png";
    device->getVideoDriver()->writeImageToFile(image, name.c_str());
    image->drop();

    return mb;

} // build

// ----------------------------------------------------------------------------
void Terrain::save(FILE* file)
{
    fwrite(&m_x, sizeof(f32), 1, file);
    fwrite(&m_z, sizeof(f32), 1, file);

    fwrite(&m_nx, sizeof(u32), 1, file);
    fwrite(&m_nz, sizeof(u32), 1, file);

    for (u32 j = 0; j < m_nz; j++)
        for (u32 i = 0; i < m_nx; i++)
        {
            fwrite(&m_mesh.vertices[m_nx*j + i].Pos.Y, sizeof(f32), 1, file);
        }
    fwrite(&SPIMG_X, sizeof(u32), 1, file);
    fwrite(&SPIMG_Y, sizeof(u32), 1, file);
    fwrite(m_material.getTexture(1)->lock(ETLM_READ_ONLY),
                      sizeof(u8), 4*SPIMG_X * SPIMG_Y, file);

    m_material.getTexture(1)->unlock();

} // save

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

    driver->drawVertexPrimitiveList(&m_mesh.vertices[0], m_mesh.vertex_count,
                                    &m_mesh.indices[0], m_mesh.quad_count * 2,
                                    video::EVT_2TCOORDS, EPT_TRIANGLES,
                                    video::EIT_16BIT);

    if (m_highlight_mesh.vertices && m_highlight_visible)
    {
        driver->setMaterial(m_highlight_material);
        driver->drawVertexPrimitiveList(&m_highlight_mesh.vertices[0],
                                         m_highlight_mesh.vertex_count,
                                         &m_highlight_mesh.indices[0],
                                         m_highlight_mesh.quad_count * 2,
                                         video::EVT_2TCOORDS, EPT_TRIANGLES,
                                         video::EIT_16BIT);
    }

} // render


// ----------------------------------------------------------------------------
void Terrain::OnSetConstants(video::IMaterialRendererServices* services, s32 userData)
{
    int tex_0 = 1;
    int tex_1 = 2;
    int tex_2 = 3;
    int tex_3 = 4;
    int tex_4 = 5;

    services->setPixelShaderConstant("splatting", &tex_0, 1);
    services->setPixelShaderConstant("terrain_1", &tex_1, 1);
    services->setPixelShaderConstant("terrain_2", &tex_2, 1);
    services->setPixelShaderConstant("terrain_3", &tex_3, 1);
    services->setPixelShaderConstant("terrain_4", &tex_4, 1);
} // OnSetConstants

