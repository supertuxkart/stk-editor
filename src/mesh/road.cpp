#include "mesh/road.hpp"

#include "editor.hpp"
#include "viewport/viewport.hpp"

#include <iostream>
#include "assert.h"

// ----------------------------------------------------------------------------
void Road::textureExport(FILE* fp)
{
    fwrite(&m_tex_warp_count, sizeof(u32), 1, fp);
    ITexture* tex = m_mesh_buff->Material.getTexture(0);
    u8 v = 1;
    if (tex)
    {
        fwrite(&v, sizeof(u8), 1, fp);
        Editor::writeStrc(fp, Editor::getTexStr(tex));
    }
    else
    {
        v = 0;
        fwrite(&v, sizeof(u8), 1, fp);
    }
} // textureExport

// ----------------------------------------------------------------------------
void Road::textureImport(FILE* fp)
{
    fread(&m_tex_warp_count, sizeof(u32), 1, fp);
    ITexture* tex;
    u8 v;
    fread(&v, sizeof(u8), 1, fp);
    if (v == 1)
    {
        Editor::readTexSt(fp, &tex);
        m_mesh_buff->Material.setTexture(0, tex);
    }
} // textureImport

// ----------------------------------------------------------------------------
void Road::crossRoadExport(FILE* fp)
{
    fwrite(&m_width_vert_num, sizeof(u32), 1, fp);

    for (u32 i = 0; i < m_width_vert_num; i++)
        fwrite(&m_cross_section[i], sizeof(vector2df), 1, fp);

} // crossRoadExport

// ----------------------------------------------------------------------------
void Road::crossRoadImport(FILE* fp)
{
    fread(&m_width_vert_num, sizeof(u32), 1, fp);
    array<vector2df> cs;

    for (u32 i = 0; i < m_width_vert_num; i++)
    {
        vector2df pos;
        fread(&pos, sizeof(vector2df), 1, fp);
        cs.push_back(pos);
    }
    setCrossSection(cs);
} // crossRoadImport

// ----------------------------------------------------------------------------
void Road::calcVertexRow(vector3df p, vector3df n, vector3df w, int offset,
                                                      float wx, float t)
{
    assert(m_district > 0);
    w *= m_width / m_width_vert_num * wx;
    for (u32 i = 0; i < m_width_vert_num; i++)
    {
        m_mesh_buff->Vertices[offset + i].Pos = 
            p + w * m_cross_section[i].X + n * m_cross_section[i].Y;

        m_mesh_buff->Vertices[offset + i].Color = 
            (m_cross_section[i].Y>0) ? SColor(255, 255, 255, 255) : SColor(255, 0, 0, 0);

        m_mesh_buff->Vertices[offset + i].TCoords = 
            vector2df(m_cross_sec_point_coords[i] / m_district, t*m_tex_warp_count);
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
            m_mesh_buff->Indices[ix] = i + 1 + (j + 1) * ni;   ix++;
            m_mesh_buff->Indices[ix] = i + (j + 1)     * ni;   ix++;
            m_mesh_buff->Indices[ix] = i + 1 + j       * ni;   ix++;

            m_mesh_buff->Indices[ix] = i + j           * ni;   ix++;
            m_mesh_buff->Indices[ix] = i + 1 + j       * ni;   ix++;
            m_mesh_buff->Indices[ix] = i + (j + 1)     * ni;   ix++;
        }

        m_mesh_buff->Indices[ix] = 0  + (j + 1)         * ni;   ix++;
        m_mesh_buff->Indices[ix] = ni - 1 + (j + 1)     * ni;   ix++;
        m_mesh_buff->Indices[ix] = 0  + j               * ni;   ix++;

        m_mesh_buff->Indices[ix] = ni - 1 + j           * ni;   ix++;
        m_mesh_buff->Indices[ix] = 0 + j                * ni;   ix++;
        m_mesh_buff->Indices[ix] = ni - 1 + (j + 1)     * ni;   ix++;
    }
} // createIndexList

// ----------------------------------------------------------------------------
Road::Road(ISceneNode* parent, ISceneManager* mgr, s32 id, ISpline* s, stringw n)
                                                    :IRoad(parent, mgr, id, s, n)
{
    m_district                            = -1;
    m_width_vert_num                      = 12;
    m_tex_warp_count                      = 10;
    m_tri                                 = 0;

    setCrossSection(genStandardCrossSection(m_width_vert_num));

    m_mesh_buff                           = new CMeshBuffer<S3DVertex2TCoords>();
    m_mesh_buff->Material.Wireframe       = true;
    m_mesh_buff->Material.Lighting        = false;
    m_mesh_buff->Material.BackfaceCulling = false;
} // road

// ----------------------------------------------------------------------------
Road::Road(ISceneNode* parent, ISceneManager* mgr, s32 id, FILE* fp)
                                         :IRoad(parent, mgr, id, fp)
{
    m_district                            = -1;
    m_tri                                 = 0;
    m_mesh_buff                           = new CMeshBuffer<S3DVertex2TCoords>();
    m_mesh_buff->Material.Wireframe       = true;
    m_mesh_buff->Material.Lighting        = false;
    m_mesh_buff->Material.BackfaceCulling = false;
    textureImport(fp);
    crossRoadImport(fp);
} // Road

// ----------------------------------------------------------------------------
void Road::refresh()
{
    assert(m_width_vert_num % 4 == 0);

    m_spline->updatePosition();
    if (m_auto_calc_norm)
        m_spline->genNormalsFromFirst();

    m_mesh_buff->setDirty();
    if (!m_spline->hasEnoughPoints()) return;

    int spn = m_spline->getPointNum() - 1;
    m_mesh_buff->Vertices.reallocate((int)(1.0f / m_detail * spn + 1) * m_width_vert_num);
    m_mesh_buff->Indices.reallocate((int)(1.0f / m_detail * spn) * m_width_vert_num * 6);

    m_mesh_buff->Vertices.set_used((int)(1.0f / m_detail * spn + 1) * m_width_vert_num);
    m_mesh_buff->Indices.set_used((int)(1.0f / m_detail * spn) * m_width_vert_num * 6);

    vector3df last_point = m_spline->p(0);
    vector3df point;
    vector3df n,v,w;

    float dt = m_detail / spn;
    point = m_spline->p(0);
    n = (m_spline->getNormal(dt) + m_spline->getNormal(0)) / 2.0f;
    v = m_spline->p(dt) - m_spline->p(0);
    w = v.crossProduct(n);

    calcVertexRow(point, n.normalize(), w.normalize(), 0, m_spline->getWidth(0),0);

    int j = 1;
    for (float t = dt; j <= (int)(1.0f / m_detail * spn); t += dt)
    {
        point  = m_spline->p(t);
        n = (m_spline->getNormal(t) + m_spline->getNormal(t-m_detail))/2.0f;
        v = point - last_point;
        w = v.crossProduct(n);

        calcVertexRow(point, n.normalize(), w.normalize(),
                     j * m_width_vert_num, m_spline->getWidth(t),t);

        last_point = point;
        j++;
    }
    createIndexList((int)(1.0f / m_detail * spn + 1), m_width_vert_num);

    m_mesh_buff->recalculateBoundingBox();
    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    sm->getMeshManipulator()->recalculateNormals(m_mesh_buff, false, false);
} // refresh

// ----------------------------------------------------------------------------
void Road::render()
{
    IVideoDriver* driver = SceneManager->getVideoDriver();
    if (m_mesh_buff->Vertices.size() == 0) return;
    driver->setTransform(ETS_WORLD, getRelativeTransformation());
    driver->setMaterial(m_mesh_buff->Material);
    driver->drawMeshBuffer(m_mesh_buff);
} // render

// ----------------------------------------------------------------------------
void Road::notify(ITexture* t)
{
    setWireFrame(false);
    m_mesh_buff->Material.setTexture(0, t);
    m_mesh_buff->Material.TextureLayer[0].TextureWrapV = ETC_REPEAT;
} // notify

// ----------------------------------------------------------------------------
void Road::setWireFrame(bool b)
{
    m_mesh_buff->Material.Wireframe = b;
    m_mesh_buff->Material.Lighting = !b;
    m_mesh_buff->Material.BackfaceCulling = !b;
    if (!b)
    {
        ISceneManager* sm = Editor::getEditor()->getSceneManager();
        m_smesh.clear();
        m_smesh.addMeshBuffer(m_mesh_buff);
        if (m_tri) m_tri->drop();
        m_tri = sm->createOctreeTriangleSelector(&m_smesh, this);
        setTriangleSelector(m_tri);
    }
} // setWireFrame

// ----------------------------------------------------------------------------
void Road::attachToDriveLine(IRoad* dl)
{
    ISpline* dl_spline = dl->getSpline();
    u32 n = m_spline->getPointNum();
    vector3df pos;
    f32 t;
    for (u32 i = 0; i < n; i++)
    {
        t = i / (f32)(n - 1);
        pos = m_spline->p(t) + m_spline->getNormal(t) * 0.5f;
        dl_spline->addControlPoint(pos,m_spline->getNormal(t));
        Viewport::get()->registerCPCreation(dl);
    }
    dl->refresh();
} // attachToDriveLine

// ----------------------------------------------------------------------------
void Road::setCrossSection(array<vector2df> cs)
{
    m_cross_section = cs;
    m_width_vert_num = cs.size();

    m_district = 0;
    m_cross_sec_point_coords.clear();
    m_cross_sec_point_coords.push_back(0.0f);
    for (u32 i = 1; i < m_width_vert_num; i++)
    {
        m_district += (m_cross_section[i] - m_cross_section[i - 1]).getLength();
        m_cross_sec_point_coords.push_back(m_district);
    }
    m_district += (m_cross_section[0] - m_cross_section[m_width_vert_num - 1]).getLength();

} // setCrossSection

// ----------------------------------------------------------------------------
array<vector2df> Road::genStandardCrossSection(u32 wvn)
{
    array<vector2df> cs;
    for (u32 i = 1; i <= wvn / 2; i++)
        cs.push_back(vector2df(-1.0f - 1 / (f32)wvn*2.0f
        + i / (f32)wvn * 4.0f, 0.5f));

    for (u32 i = wvn / 2; i >= 1; i--)
        cs.push_back(vector2df(-1.0f - 1 / (f32)wvn*2.0f
        + i / (f32)wvn * 4.0f, -0.5f));
    return cs;
} // genStandardCrossSection

// ----------------------------------------------------------------------------
stringc Road::getTexName()
{
    return Editor::toRelative(m_mesh_buff->Material.getTexture(0)->getName());
} // getTexName
