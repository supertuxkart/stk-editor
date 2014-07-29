#ifndef ISPLINE_HPP
#define ISPLINE_HPP

#include "mesh/mesh_structs.hpp"

#include <irrlicht.h>



using namespace irr;
using namespace scene;
using namespace core;

struct ControlPoint
{
    vector3df        vel;
    vector3df        pos;
    vector3df        normal;
    f32              width;
    f32              t;
    ISceneNode*      node;
    ISceneNode*      normal_node;
    ISceneNode*      width_node;
};

class ISpline :public ISceneNode
{
// ----------------------------------------------------------------------------
    // variables

private:
    bool                m_valid;
    aabbox3d<f32>       m_bounding_box;
    stringw             m_type;
protected:
    list<ControlPoint>  m_control_points;
    u32                 m_cp_num;

// ----------------------------------------------------------------------------
    // functions
protected:
    vector3df       calculateVelInPoint(vector3df pm1, vector3df p0, vector3df p1,
                                        list<ControlPoint>::Iterator it);
    void            calculateVelocity();
    ControlPoint    newControlPoint(vector3df p);
    void            saveNode(ISceneNode* sn, FILE* fp);
    void            loadNode(ISceneNode* sn, FILE* fp);

// ----------------------------------------------------------------------------
public:
    ISpline(ISceneNode* parent, ISceneManager* mgr, s32 id, stringw type);
    ISpline(ISceneNode* parent, ISceneManager* mgr, s32 id, stringw t, FILE* fp);

    virtual vector3df   p(float t) = 0;
    virtual vector3df   getNormal(float t) = 0;
    virtual float       getWidth(float t) = 0;
    void                setNodeVisibility(bool visible);
    u32                 addControlPoint(vector3df p);
    u32                 insertControlPoint(vector3df p);
    void                addControlPoint(ControlPoint cp);
    void                addControlPoint(ControlPoint cp, u32 pos);
    ControlPoint        removeControlPoint(u32 ix);
    ISceneNode*         getNode(u32 ix);
    void                updatePosition();
    void                clear();
    void                save(FILE* fp);
    u32                 getCPIndexFromNodeID(s32 id);
    bool                hasEnoughPoints()   { return m_cp_num > 1; }
    int                 getPointNum()       { return m_cp_num;     }
    bool                isValid()           { return m_valid;      }

// ----------------------------------------------------------------------------
    // inherited functions - ISceneNode
    void                 OnRegisterSceneNode();
    void                 render();

    const aabbox3d<f32>& getBoundingBox()   const { return m_bounding_box; }
    u32                  getMaterialCount() const { return 1; }
};
#endif
