#include "commands/iobject_cmd.hpp"

#include "editor.hpp"

#include <assert.h>

// ----------------------------------------------------------------------------
// IOCommand  -------------------------------------------------------------------
// ----------------------------------------------------------------------------
void IObjectCmd::redo()
{
    list<ISceneNode*>::Iterator it;
    for (it = m_elements.begin(); it != m_elements.end(); it++)
        redo((*it));
} // redo

// ----------------------------------------------------------------------------
void IObjectCmd::undo()
{
    list<ISceneNode*>::Iterator it;
    for (it = m_elements.begin(); it != m_elements.end(); it++)
        undo((*it));
} // undo

// ----------------------------------------------------------------------------
// DelCmd  --------------------------------------------------------------------
// ----------------------------------------------------------------------------
DelCmd::~DelCmd()
{
    if (m_rdy)
    {
        list<ISceneNode*>::Iterator it;
        for (it = m_elements.begin(); it != m_elements.end(); it++)
        {
            (*it)->remove();
        }
    }
} // ~DelCmd

// ----------------------------------------------------------------------------
// CreateCmd  --------------------------------------------------------------------
// ----------------------------------------------------------------------------
CreateCmd::~CreateCmd()
{
    if (!m_rdy)
    {
        list<ISceneNode*>::Iterator it;
        for (it = m_elements.begin(); it != m_elements.end(); it++)
        {
            (*it)->remove();
        }
    }
} // ~CreateCmd


// ----------------------------------------------------------------------------
// MoveCmd  -------------------------------------------------------------------
// ----------------------------------------------------------------------------

MoveCmd::MoveCmd(list<ISceneNode*> e, s32 sx, s32 sy) :IObjectCmd(e)
{
    ISceneCollisionManager* iscm;
    iscm = Editor::getEditor()->getSceneManager()->getSceneCollisionManager();
    m_start_ray = iscm->getRayFromScreenCoordinates(vector2d<s32>(sx, sy));
    m_curr_ray = m_start_ray;
} // MoveCmd

// ----------------------------------------------------------------------------
void MoveCmd::update(s32 x, s32 y)
{ 
    ISceneCollisionManager* iscm;
    iscm = Editor::getEditor()->getSceneManager()->getSceneCollisionManager();
    m_curr_ray = iscm->getRayFromScreenCoordinates(vector2d<s32>(x, y));
} // update

// ----------------------------------------------------------------------------
void MoveCmd::redo(ISceneNode *node)
{
    vector3df d = m_curr_ray.start - m_start_ray.start;
    node->setPosition(node->getPosition() + d);
} // redo

// ----------------------------------------------------------------------------
void MoveCmd::undo(ISceneNode *node)
{
    vector3df d = m_curr_ray.start - m_start_ray.start;
    if (d.getLength()>0)
        node->setPosition(node->getPosition() - d);
} // undo

// ----------------------------------------------------------------------------
// RotateCmd  -----------------------------------------------------------------
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
vector3df RotateCmd::calcRot(f32 angle, vector3df axis)
{
    quaternion q;
    vector3df rot;
    angle = degToRad(angle);
    q.fromAngleAxis(angle, axis);
    q.toEuler(rot);
    rot = vector3df(radToDeg(rot.X), radToDeg(rot.Y), radToDeg(rot.Z));
    return rot;
} // calcRot

// ----------------------------------------------------------------------------
void RotateCmd::calcCenter()
{
    ISceneCollisionManager* iscm;
    iscm = Editor::getEditor()->getSceneManager()->getSceneCollisionManager();
    list<ISceneNode*>::Iterator it;
    vector2d<s32> sp;
    for (it = m_elements.begin(); it != m_elements.end(); it++)
    {
        (*it)->updateAbsolutePosition();
        sp = iscm->getScreenCoordinatesFrom3DPosition((*it)->getTransformedBoundingBox().getCenter());
        m_cx += sp.X;
        m_cy += sp.Y;
    }
    assert(m_elements.size() > 0);
    m_cx /= m_elements.size();
    m_cy /= m_elements.size();
} // calcCenter

// ----------------------------------------------------------------------------
void RotateCmd::calcAllRot(vector3df* x, vector3df* y, vector3df* z)
{
    vector3df rotx, roty, rotz;
    if (z_mode)
    {
        vector2df v1 = vector2df(m_cx - m_sx, m_cy - m_sy);
        vector2df v2 = vector2df(m_cx - m_x, m_cy - m_y);
        v1.normalize();
        v2.normalize();
        f32 angle = atan2(v2.Y, v2.X) - atan2(v1.Y, v1.X);
        rotz = calcRot(radToDeg(angle), m_tz);
    }
    else
    {
        rotx = calcRot(m_x - m_sx, m_tx);
        roty = calcRot(m_y - m_sy, m_ty);
    }
    *x = rotx; *y = roty; *z = rotz;
} // calcAllRot

// ----------------------------------------------------------------------------
RotateCmd::RotateCmd(list<ISceneNode*> e, s32 x, s32 y,
    vector3df i, vector3df j, vector3df k) :IObjectCmd(e)
{
    m_sx = x;
    m_sy = y;
    m_x = x;
    m_y = y;
    m_tx = j;
    m_ty = -i;
    m_tz = k;
    m_cx = 0;
    m_cy = 0;
    setZMode(false, x,y);
} // RotateCmd

// ----------------------------------------------------------------------------
void RotateCmd::update(s32 x, s32 y)
{
    m_x = x;
    m_y = y;
} // update

// ----------------------------------------------------------------------------
void RotateCmd::setZMode(bool zmode, s32 x, s32 y)
{
    m_sx = x; m_sy = y; z_mode = zmode;
    if (z_mode) calcCenter();
} // switchMode

// ----------------------------------------------------------------------------
void RotateCmd::redo(ISceneNode *node)
{
    vector3df rotx, roty, rotz;
    calcAllRot(&rotx, &roty, &rotz);
    vector3df pos = node->getPosition();
    node->setPosition(vector3df(0, 0, 0));
    node->setRotation(node->getRotation() + rotx + roty + rotz);
    node->setPosition(pos);
} // redo

// ----------------------------------------------------------------------------
void RotateCmd::undo(ISceneNode *node)
{
    vector3df rotx, roty, rotz;
    calcAllRot(&rotx, &roty, &rotz);
    vector3df pos = node->getPosition();
    node->setPosition(vector3df(0, 0, 0));
    node->setRotation(node->getRotation() - rotx - roty - rotz);
    node->setPosition(pos);
} // undo

// ----------------------------------------------------------------------------
// ScaleCmd  -----------------------------------------------------------------
// ----------------------------------------------------------------------------

ScaleCmd::ScaleCmd(list<ISceneNode*> e, bool limited) :IObjectCmd(e)
{
    m_dx = 0; m_dy = 0; m_dz = 0;
    m_limited = limited;
}

// ----------------------------------------------------------------------------
void ScaleCmd::redo(ISceneNode *node)
{
    float dx, dy, dz;
    dx = m_dx; dy = m_dy; dz = m_dz;
    if (m_limited)
    {
        dx = dx + dy + dz;
        dy = dx;
        dz = dx;
    }
    node->setScale(node->getScale() + vector3df(dz / 10.0f, dy / 10.0f, dx / 10.0f));
} // redo

// ----------------------------------------------------------------------------
void ScaleCmd::undo(ISceneNode *node)
{
    float dx, dy, dz;
    dx = m_dx; dy = m_dy; dz = m_dz;
    if (m_limited)
    {
        dx = dx + dy + dz;
        dy = dx;
        dz = dx;
    }
    node->setScale(node->getScale() + vector3df(-dz / 10.0f, -dy / 10.0f, -dx/ 10.0f));
} // undo

