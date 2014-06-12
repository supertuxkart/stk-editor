#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace video;
using namespace core;

class Terrain :public ISceneNode
{
public:
    struct TerrainMod
    {
        int   edge_type;
        int   ID;
        float radius;
        float dh;
        long  countdown;
        bool  max;
        float max_v;
        bool  min;
        float min_v;
        bool  max_cut;
        float cat_v;
    };

    struct Mesh
    {
        S3DVertex*      vertices;
        u16*            indices;
        unsigned int    vertex_count;
        unsigned int    quad_count;
    };

private:
    SMaterial       m_material;

    Mesh            m_mesh;

    f32             m_x, m_z;
    int             m_nx, m_nz;

    int             m_last_mod_ID;
    float*          m_vertex_h_before;

    Mesh            m_highlight_mesh;

    aabbox3d<f32>   m_bounding_box;

    void            coinAroundIntersection(line3d<float> ray, float r, vector2df* cpos,
                                           int* ix, int* iz, int* dx, int* dz);
    void            createIndexList(u16* indices, int x, int z);

public:
    Terrain(ISceneNode* parent, ISceneManager* mgr, s32 id);
    ~Terrain();

    void                         setSize(float x, float y, int nx, int nz);
    void                         modify(line3d<float> ray, const TerrainMod& tm);
    void                         cut(line3d<float> ray, const TerrainMod& tm);
    void                         highlight(line3d<float> ray, float radius);

    virtual void                 OnRegisterSceneNode();
    virtual void                 render();

    virtual const aabbox3d<f32>& getBoundingBox()   const { return m_bounding_box; }
    virtual u32                  getMaterialCount() const { return 1; }
    virtual SMaterial&           getMaterial(u32 i)       { return m_material; }

};

#endif
