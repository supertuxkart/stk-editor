#ifndef IROAD_HPP
#define IROAD_HPP

#include "spline/ispline.hpp"
#include "mesh/mesh_structs.hpp"

class IRoad :public ISceneNode
{
protected:
    ISpline*        m_spline;
    Mesh            m_mesh;

    f32             m_detail;
    f32             m_width;
    u32             m_width_vert_num;

    SMaterial       m_material;
    u32             m_tex_warp_count;

    aabbox3d<f32>   m_bounding_box;

public:
    IRoad(ISceneNode* parent, ISceneManager* mgr, s32 id, ISpline* s, stringw n);

    IRoad(ISceneNode* parent, ISceneManager* mgr, s32 id, FILE* fp);

    virtual void    refresh() = 0;
    void            clear();

    void            setDetail(float d)      { m_detail = d; refresh();         }
    void            setWidth(float d)       { m_width =  d; refresh();         }
    void            setTexWrapCount(u32 c)  { m_tex_warp_count = c; refresh(); }

    ISpline*        getSpline()             { return m_spline; }

    virtual void    save(FILE* fp);

// ----------------------------------------------------------------------------
    // inherited functions - ISceneNode
    void                 OnRegisterSceneNode();
    virtual void         render() = 0;

    const aabbox3d<f32>& getBoundingBox()   const { return m_bounding_box; }
    u32                  getMaterialCount() const { return 1; }

};

#endif
