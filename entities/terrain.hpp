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
    };

private:
    SMaterial       m_material;
    
    S3DVertex*      m_vertices;
    u16*            m_indices;
    unsigned int    m_vertex_count;
    unsigned int    m_quad_count;
    f32             m_x, m_z;
    int             m_nx, m_nz;

    int             m_last_mod_ID;
    float*          m_vertex_h_before;

    aabbox3d<f32>   m_bounding_box;

public:
    Terrain(ISceneNode* parent, ISceneManager* mgr, s32 id);
    ~Terrain();

    void         setSize(float x, float y, int nx, int nz);

    void         modify(line3d<float> ray, TerrainMod tm);

    virtual void OnRegisterSceneNode();
    virtual void render();

    virtual const aabbox3d<f32>& getBoundingBox() const   { return m_bounding_box; }

    virtual u32 getMaterialCount() const
        {return 1;}

    virtual SMaterial& getMaterial(u32 i)
        {return m_material;}

};

#endif
