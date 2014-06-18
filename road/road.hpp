#ifndef ROAD_HPP
#define ROAD_HPP

#include "road/ispline.hpp"

#include "structs.hpp"

class Road :public ISceneNode
{
private:
    ISpline*        m_spline;
    Mesh            m_mesh;

    float           m_detail;
    float           m_width;
    int             m_width_vert_num;
    
    SMaterial       m_material;

    aabbox3d<f32>   m_bounding_box;


    void        calcVertexRow(vector3df p, vector3df n, vector3df w, int offset);
    void        createIndexList(int nj, int ni);

public:
    Road(ISceneNode* parent, ISceneManager* mgr, s32 id, ISpline* s);

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
