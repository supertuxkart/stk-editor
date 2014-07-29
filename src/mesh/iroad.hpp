#ifndef IROAD_HPP
#define IROAD_HPP

#include "spline/ispline.hpp"
#include "mesh/mesh_structs.hpp"

class IRoad :public ISceneNode
{
protected:
    bool            m_valid;
    ISpline*        m_spline;

    f32             m_detail;
    f32             m_width;

    u32             m_width_vert_num;

    u32             m_tex_warp_count;

    aabbox3d<f32>   m_bounding_box;

    virtual void    textureExport(FILE* fp) = 0;
public:
    IRoad(ISceneNode* parent, ISceneManager* mgr, s32 id, ISpline* s, stringw n);
    IRoad(ISceneNode* parent, ISceneManager* mgr, s32 id, FILE* fp);

    virtual void    refresh() = 0;
    void            clear();


    void    setDetail(float d)    { m_detail = d; refresh(); setWireFrame(true);         }
    void    setWidth(float d)     { m_width = d; refresh(); setWireFrame(true);          }
    void    setTexWrapCount(u32 c){ m_tex_warp_count = c; refresh(); setWireFrame(false);}

    bool            isValid()               { return m_valid;  }
    ISpline*        getSpline()             { return m_spline; }

    virtual void    save(FILE* fp);
    virtual void    setWireFrame(bool b)    {};

// ----------------------------------------------------------------------------
    // inherited functions - ISceneNode
    void                 OnRegisterSceneNode();
    virtual void         render() = 0;

    virtual const aabbox3d<f32>& getBoundingBox()   const { return m_bounding_box; }
    u32                          getMaterialCount() const { return 1; }

};

#endif
