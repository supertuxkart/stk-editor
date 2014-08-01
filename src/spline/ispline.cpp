#include "spline/ispline.hpp"
#include "editor.hpp"

#define MAX_SPLINE_CP_NUM 3000

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
void ISpline::saveNode(ISceneNode* sn, FILE* fp)
{
    fwrite(&sn->getPosition(), sizeof(vector3df), 1, fp);
    fwrite(&sn->getRotation(), sizeof(vector3df), 1, fp);
    fwrite(&sn->getScale(),    sizeof(vector3df), 1, fp);
} // saveNode

// ----------------------------------------------------------------------------
void ISpline::loadNode(ISceneNode* sn, FILE* fp)
{
    vector3df pos, rot, sca;
    fread(&pos, sizeof(vector3df), 1, fp);
    fread(&rot, sizeof(vector3df), 1, fp);
    fread(&sca, sizeof(vector3df), 1, fp);

    sn->setPosition(pos);
    sn->setRotation(rot);
    sn->setScale(sca);

} // loadNode

// ----------------------------------------------------------------------------
ISpline::ISpline(ISceneNode* parent, ISceneManager* mgr, s32 id, stringw type)
                                                    :ISceneNode(parent, mgr, id)
{
    m_valid = true;
    m_type = type;
    setAutomaticCulling(EAC_OFF);
    m_cp_num = 0;
} // ISpline

// ----------------------------------------------------------------------------
ISpline::ISpline(ISceneNode* parent, ISceneManager* mgr, s32 id, stringw t, FILE* fp)
                                                         :ISceneNode(parent, mgr, id)
{
    m_valid = true;
    m_type = t;
    u32 size;
    fread(&size, sizeof(u32), 1, fp);
    if (size > MAX_SPLINE_CP_NUM)
    {
        m_valid = false;
        return;
    }
    list<ControlPoint>::Iterator it;
    m_cp_num = 0;
    for (u32 i = 0; i < size; i++)
    {
        m_control_points.push_back(newControlPoint(vector3df(0, 0, 0)));
        it = m_control_points.getLast();
        loadNode(it->node, fp);
        loadNode(it->normal_node, fp);
        loadNode(it->width_node, fp);
        fread(&it->t, sizeof(f32), 1, fp);
    }
    updatePosition();
    setAutomaticCulling(EAC_OFF);
} // ISpline

// ----------------------------------------------------------------------------
void ISpline::setNodeVisibility(bool visible)
{
    setVisible(visible);
    /*
    list<ControlPoint>::Iterator it;
    for (it = m_control_points.begin(); it != m_control_points.end(); it++)
        it->node->setVisible(visible);
    */
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

// ----------------------------------------------------------------------------
void ISpline::addControlPoint(ControlPoint cp)
{
    cp.node->setVisible(true);
    cp.normal_node->setVisible(true);
    cp.width_node->setVisible(true);
    m_control_points.push_back(cp);
    m_cp_num++;
} // addControlPoint

// ----------------------------------------------------------------------------
void ISpline::addControlPoint(ControlPoint cp, u32 pos)
{
    cp.node->setVisible(true);
    cp.normal_node->setVisible(true);
    cp.width_node->setVisible(true);

    if (m_control_points.size() == pos)
    {
        m_control_points.push_back(cp);
    }
    else
    {
        list<ControlPoint>::Iterator it = m_control_points.begin();
        for (u32 i = 0; i < pos; i++, it++);
        m_control_points.insert_before(it, cp);
    }
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
    if (ix >= m_cp_num) return 0;
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
void ISpline::clear()
{
    list<ControlPoint>::Iterator it = m_control_points.begin();
    for (; it != m_control_points.end(); it++)
    {
        it->node->remove();
        it->normal_node->remove();
        it->width_node->remove();
    }
} // clear

// ----------------------------------------------------------------------------
void ISpline::save(FILE* fp)
{
    u8 size = m_type.size() + 1;
    fwrite(&size, sizeof(u8), 1, fp);
    fwrite(m_type.c_str(), sizeof(wchar_t), size, fp);
    
    list<ControlPoint>::Iterator it = m_control_points.begin();

    fwrite(&m_cp_num, sizeof(u32), 1, fp);
    for (u32 i = 0; i < m_cp_num; i++, it++)
    {
        saveNode(it->node, fp);
        saveNode(it->normal_node, fp);
        saveNode(it->width_node, fp);
        fwrite(&it->t, sizeof(f32), 1, fp);
    }

} // save

// ----------------------------------------------------------------------------
u32 ISpline::getCPIndexFromNodeID(s32 id)
{
    list<ControlPoint>::Iterator it = m_control_points.begin();    
    int ix = 0;
    while (it!=m_control_points.end())
    {
        if (it->node->getID() == id) return ix;
        ix++;
        it++;
    }
    return -1;
} // getCPIndexFromNodeID

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
    driver->setTransform(ETS_WORLD, getAbsoluteTransformation());

    float dt = 0.005f;
    for (float t = dt; t <= 1.0; t += dt)
    {
        driver->draw3DLine(p(t - dt), p(t), SColor(255, 255, 0, 0));
    }

} // render
