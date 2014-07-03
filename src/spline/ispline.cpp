#include "spline/ispline.hpp"
#include "editor.hpp"

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
    if (m_cp_num < 2) return;

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
    cp.width  = 1.0f;
    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    cp.node = sm->addSphereSceneNode(0.2f, 16,this,
        ANOTHER_MAGIC_NUMBER + 3 * m_cp_num, p);
    cp.node->getMaterial(0).DiffuseColor = SColor(255, 255, 0, 0);
    cp.node->getMaterial(0).AmbientColor = SColor(255, 255, 0, 0);

    cp.normal_node = sm->addSphereSceneNode(0.1f, 16, cp.node,
        ANOTHER_MAGIC_NUMBER + 3 * m_cp_num + 1,vector3df(0,1,0));
    cp.normal_node->getMaterial(0).DiffuseColor = SColor(255, 0, 255, 0);
    cp.normal_node->getMaterial(0).AmbientColor = SColor(255, 0, 255, 0);

    cp.width_node = sm->addSphereSceneNode(0.1f, 16, cp.node,
        ANOTHER_MAGIC_NUMBER + 3 * m_cp_num + 2, vector3df(1, 0, 0));
    cp.width_node->getMaterial(0).DiffuseColor = SColor(255, 0, 0, 255);
    cp.width_node->getMaterial(0).AmbientColor = SColor(255, 0, 0, 255);

    return cp;

} // newControlPoint

// ----------------------------------------------------------------------------
ISpline::ISpline(ISceneNode* parent, ISceneManager* mgr, s32 id)
                                                    :ISceneNode(parent, mgr, id)
{
    setAutomaticCulling(EAC_OFF);
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
u32 ISpline::addControlPoint(vector3df p)
{
    m_control_points.push_back(newControlPoint(p));
    calculateVelocity();

    return m_cp_num - 1;

} // addControlPoint

// ----------------------------------------------------------------------------
u32 ISpline::insertControlPoint(vector3df p)
{

    if (m_control_points.empty())
    {
        m_control_points.push_back(newControlPoint(p));
        return 0;
    }


    u32 ix = 0;
    u32 fix = 0;

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
            fix = ix;
        }
        ix++;
    }

    bool insert_before;
    it = min_index;

    if (it == m_control_points.begin())
    {
        // before or after the first point
        if (m_control_points.size() > 1 
            && (it + 1)->pos.getDistanceFrom(p) > (it)->pos.getDistanceFrom(p))
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
    else
    {
        m_control_points.insert_after(it, newControlPoint(p));
        fix++;
    }

    calculateVelocity();
    return fix;
} // insertControlPoint

void ISpline::addControlPoint(ControlPoint cp)
{
    cp.node->setVisible(true);
    cp.normal_node->setVisible(true);
    cp.width_node->setVisible(true);
    m_control_points.push_back(cp);
    m_cp_num++;
} // addControlPoint

void ISpline::addControlPoint(ControlPoint cp, u32 pos)
{
    cp.node->setVisible(true);
    cp.normal_node->setVisible(true);
    cp.width_node->setVisible(true);

    list<ControlPoint>::Iterator it = m_control_points.begin();
    for (u32 i = 0; i < pos; i++, it++);
    m_control_points.insert_before(it,cp);
    m_cp_num++;
} // addControlPoint

// ----------------------------------------------------------------------------
ControlPoint ISpline::removeControlPoint(u32 ix)
{
    list<ControlPoint>::Iterator it = m_control_points.begin();
    for (u32 i = 0; i < ix; i++, it++);
    
    ControlPoint cp = *it;
    cp.normal_node->setVisible(false);
    cp.width_node->setVisible(false);
    cp.node->setVisible(false);

    m_control_points.erase(it);
    m_cp_num--;
    calculateVelocity();

    return cp;

} // removeControlPoint

// ----------------------------------------------------------------------------
ISceneNode* ISpline::getNode(u32 ix)
{
    list<ControlPoint>::Iterator it = m_control_points.begin();
    for (u32 i = 0; i < ix; i++, it++);
    return it->node;

} // getNode

// ----------------------------------------------------------------------------
void ISpline::updatePosition()
{
    if (m_cp_num < 1) return;
    list<ControlPoint>::Iterator it;
    vector3df v;
    for (it = m_control_points.begin(); it != m_control_points.end(); it++)
    {
        v = it->normal_node->getPosition();
        if (v.getLength() > 1.0)
        {
            v.normalize();
            it->normal_node->setPosition(v);
        }
        it->pos = it->node->getPosition() + getPosition();
        it->normal = it->normal_node->getPosition();
        it->normal.normalize();

        v = it->width_node->getPosition();
        it->width = v.X;
        it->width_node->setPosition(vector3df(v.X, 0.0f, 0.0f));
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


    float dt = 0.005f;
    for (float t = dt; t <= 1.0; t += dt)
    {
        driver->draw3DLine(p(t - dt), p(t), SColor(255, 255, 0, 0));
    }

} // render
