#ifndef DRIVELINE_HPP
#define DRIVELINE_HPP

#include "road/ispline.hpp"

#include "structs.hpp"

class DriveLine :public ISceneNode
{
private:
    ISpline*        m_spline;
    Mesh            m_mesh;

    float           m_detail;
    float           m_width;

    SMaterial       m_material;

    aabbox3d<f32>   m_bounding_box;


public:
    DriveLine(ISceneNode* parent, ISceneManager* mgr, s32 id, ISpline* s);

    void    refresh();

    void    setDetail(float d)      { m_detail = d; }

    // ----------------------------------------------------------------------------
    // inherited functions - ISceneNode
    void                 OnRegisterSceneNode();
    void                 render();

    const aabbox3d<f32>& getBoundingBox()   const { return m_bounding_box; }
    u32                  getMaterialCount() const { return 1; }

};

#endif
