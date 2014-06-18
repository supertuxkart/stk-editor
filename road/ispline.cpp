#include "road/ispline.hpp"
#include "editor.hpp"

#include <assert.h>

// ----------------------------------------------------------------------------
vector3df ISpline::calculateVelInPoint(vector3df pm, vector3df p0, vector3df pp,
                                  list<ControlPoint>::Iterator it)
{
    if (it == m_control_points.begin() || (it + 1) == m_control_points.end())
        return vector3df(0, 0, 0);
    float t0 = it->t;
    float tm = (it - 1)->t;
    float tp = (it + 1)->t;
    return ((pp - p0) / (tp - t0) + (p0 - pm) / (t0 - tm)) / 2.0 * 1.5;
} // calculateVelInPoint

// ----------------------------------------------------------------------------
void ISpline::calculateVelocity()
{
    list<ControlPoint>::Iterator it = m_control_points.begin();
    it->vel = vector3df(0.0f, 0.0f, 0.0f);

    for (int i = 0; it != m_control_points.end(); it++, i++)
    {
        it->t = i / (float)(m_cp_num - 1);
    }

    if (m_cp_num < 2) return;

    vector3df pm, p0, pp;
    for (it = m_control_points.begin() + 1; (it + 1) != m_control_points.end(); it++)
    {
        p0 = it->pos;      
        pm = (it - 1)->pos;
        pp = (it + 1)->pos;
        it->vel = calculateVelInPoint(pm, p0, pp, it);
    }
    it->vel = vector3df(0.0f, 0.0f, 0.0f);

} // calculateVelocity

// ----------------------------------------------------------------------------
ControlPoint ISpline::newControlPoint(vector3df p)
{
    m_cp_num++;
    ControlPoint cp;
    cp.pos = p;
    cp.normal = vector3df(0.0f, 1.0f, 0.0f);
    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    cp.node = sm->addSphereSceneNode(0.2, 16, 0, ANOTHER_MAGIC_NUMBER + 2 * m_cp_num, p);
    cp.normal_node = sm->addSphereSceneNode(0.2, 16, cp.node, ANOTHER_MAGIC_NUMBER + 2 * m_cp_num + 1,vector3df(0,1,0));
    cp.normal_node->getMaterial(0).DiffuseColor = SColor(255, 0, 255, 0);
    cp.normal_node->getMaterial(0).AmbientColor = SColor(255, 0, 255, 0);

    return cp;

} // newControlPoint

// ----------------------------------------------------------------------------
ISpline::ISpline(ISceneNode* parent, ISceneManager* mgr, s32 id) 
                                                    :ISceneNode(parent, mgr, id)
{
    m_cp_num = 0;
} // ISpline

// ----------------------------------------------------------------------------
void ISpline::setNodeVisibility(bool visible)
{
    list<ControlPoint>::Iterator it;
    for (it = m_control_points.begin(); it != m_control_points.end(); it++)
        it->node->setVisible(visible);
} // showNodes


// ----------------------------------------------------------------------------
void ISpline::addControlPoint(vector3df p)
{
    m_control_points.push_back(newControlPoint(p));
    calculateVelocity();
} // addControlPoint

// ----------------------------------------------------------------------------
void ISpline::insertControlPoint(vector3df p)
{

    list<ControlPoint>::Iterator it;
    list<ControlPoint>::Iterator min_index = m_control_points.begin();
    float min_value = 999999;

    // search for the closest point
    for (it = min_index; it != m_control_points.end(); it++)
    {
        if (it->pos.getDistanceFrom(p) < min_value)
        {
            min_value = it->pos.getDistanceFrom(p);
            min_index = it;
        }
    }

    bool insert_before;
    it = min_index;

    if (it == m_control_points.begin())
    {
        // before or after the first point
        if ((it + 1)->pos.getDistanceFrom(p) > (it)->pos.getDistanceFrom(p))
            insert_before = true;
        else insert_before = false;
    }
    else if ((it + 1) == m_control_points.end())
    {
        // before or after the last point
        if (it->pos.getDistanceFrom(p) > (it-1)->pos.getDistanceFrom(p))
            insert_before = true;
        else insert_before = false;
    }
    else if ((it + 1)->pos.getDistanceFrom(p) > (it - 1)->pos.getDistanceFrom(p))
    {
        insert_before = true;
    } else insert_before = false;
    
    if (insert_before) m_control_points.insert_before(it, newControlPoint(p));
    else               m_control_points.insert_after(it, newControlPoint(p));
    
    calculateVelocity();

} // insertControlPoint

// ----------------------------------------------------------------------------
void ISpline::updatePosition()
{
    list<ControlPoint>::Iterator it;

    for (it = m_control_points.begin(); it != m_control_points.end(); it++)
    {
        it->pos = it->node->getPosition();
        it->normal = it->normal_node->getPosition();
        it->normal.normalize();
    }
    calculateVelocity();
} // updatePosition


// ----------------------------------------------------------------------------
void ISpline::OnRegisterSceneNode()
{
    if (IsVisible)
        SceneManager->registerNodeForRendering(this);

    ISceneNode::OnRegisterSceneNode();

} // OnRegisterSceneNode


// ----------------------------------------------------------------------------
void ISpline::render()
{

    if (m_cp_num < 2) return;

    IVideoDriver* driver = SceneManager->getVideoDriver();

    SMaterial material;

    material.Lighting = false;

    driver->setMaterial(material);
    driver->setTransform(ETS_WORLD, IdentityMatrix);
   

    float dt = 0.005;
    for (float t = dt; t <= 1.0; t += dt)
    {
        driver->draw3DLine(p(t - dt), p(t), SColor(255, 255, 0, 0));
    }

} // render
