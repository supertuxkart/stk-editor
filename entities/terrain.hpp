#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace video;
using namespace core;

class Terrain :public ISceneNode
{
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

public:
    Terrain(ISceneNode* parent, ISceneManager* mgr, s32 id);
    ~Terrain();

    void         setSize(float x, float y, int nx, int nz);

    void         modify(line3d<float> ray, float radius, float dh, int MID);

    virtual void OnRegisterSceneNode();
    virtual void render();

    virtual const aabbox3d<f32>& getBoundingBox() const
        {return aabbox3d<f32>(0,0,0,m_x,1,m_z);}

    virtual u32 getMaterialCount() const
        {return 1;}

    virtual SMaterial& getMaterial(u32 i)
        {return m_material;}

};

#endif
