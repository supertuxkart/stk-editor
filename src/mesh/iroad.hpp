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

    bool            m_auto_calc_norm;

    aabbox3d<f32>   m_bounding_box;
    virtual void    textureExport(FILE* fp)   = 0;
    virtual void    crossRoadExport(FILE* fp) {};

public:
    IRoad(ISceneNode* parent, ISceneManager* mgr, s32 id, ISpline* s, stringw n);
    IRoad(ISceneNode* parent, ISceneManager* mgr, s32 id, FILE* fp);

    virtual void    refresh() = 0;
    void            clear();

    void    setAutoNorm(bool b)   { m_auto_calc_norm = b;    refresh();          }
    void    setDetail(float d)    { m_detail = d; refresh(); setWireFrame(true); }
    void    setWidth(float d)     { m_width = d;  refresh(); setWireFrame(true); }

    bool      getAutoNorm()       { return m_auto_calc_norm; }
    bool      isValid()           { return m_valid;          }
    ISpline*  getSpline()         { return m_spline;         }

    virtual void    save(FILE* fp);
    virtual void    setWireFrame(bool b)    {};
    virtual void    setTexWrapCount(u32 twc){};

// ----------------------------------------------------------------------------
    // inherited functions - ISceneNode
    void                 OnRegisterSceneNode();
    virtual void         render() = 0;

    virtual const aabbox3d<f32>& getBoundingBox()   const { return m_bounding_box; }
    u32                          getMaterialCount() const { return 1; }

};

#endif
