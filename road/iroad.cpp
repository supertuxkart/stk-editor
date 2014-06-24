#include "road/iroad.hpp"

#include <iostream>
#include "assert.h"

// ----------------------------------------------------------------------------
IRoad::IRoad(ISceneNode* parent, ISceneManager* mgr, s32 id, ISpline* s)
                                :ISceneNode(parent, mgr, id)
{
    m_spline            = s;
    m_mesh.indices      = 0;
    m_mesh.vertices     = 0;
    m_mesh.vertex_count = 0;
    m_mesh.quad_count   = 0;
    m_detail            = 0.25f;
    m_width             = 10.0f;
    m_width_vert_num    = 12;

    m_material.Wireframe       = true;
    m_material.Lighting        = false;
    m_material.BackfaceCulling = false;

} // Road

// ----------------------------------------------------------------------------
void IRoad::OnRegisterSceneNode()
{
    if (IsVisible)
        SceneManager->registerNodeForRendering(this);

    ISceneNode::OnRegisterSceneNode();

} // OnRegisterSceneNode



