#include "commands/iobject_cmd.hpp"

#include "editor.hpp"

#include "mesh/iroad.hpp"

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
void IObjectCmd::calcCenter(f32 *x, f32 *y)
{ 
    (*x) = 0; (*y) = 0;
    ISceneCollisionManager* iscm;
    iscm = Editor::getEditor()->getSceneManager()->getSceneCollisionManager();
    list<ISceneNode*>::Iterator it;
    vector2d<s32> sp;
    for (it = m_elements.begin(); it != m_elements.end(); it++)
    {
        (*it)->updateAbsolutePosition();
        sp = iscm->getScreenCoordinatesFrom3DPosition((*it)->getTransformedBoundingBox().getCenter());
        (*x) += sp.X;
        (*y) += sp.Y;
    }
    assert(m_elements.size() > 0);
    (*x) /= m_elements.size();
    (*y) /= m_elements.size();
} // calcCenter

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
MoveCmd::MoveCmd(list<ISceneNode*> e, s32 sx, s32 sy, IRoad* r) :IObjectCmd(e)
{
    m_road = r;
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
    if (m_road) m_road->refresh();
} // redo

// ----------------------------------------------------------------------------
void MoveCmd::undo(ISceneNode *node)
{
    vector3df d = m_curr_ray.start - m_start_ray.start;
    if (d.getLength()>0)
        node->setPosition(node->getPosition() - d);
    if (m_road) m_road->refresh();
} // undo

// ----------------------------------------------------------------------------
// RotateCmd  -----------------------------------------------------------------
// ----------------------------------------------------------------------------

bool RotateCmd::Z_MODE = false;

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
void RotateCmd::calcAllRot(vector3df* x, vector3df* y, vector3df* z)
{
    vector3df rotx, roty, rotz;
    if (Z_MODE)
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
    m_sx = (f32) x;
    m_sy = (f32) y;
    m_x  = (f32) x;
    m_y  = (f32) y;
    m_tx = j;
    m_ty = -i;
    m_tz = k;
    m_cx = 0;
    m_cy = 0;
    if (Z_MODE) calcCenter(&m_cx, &m_cy);
} // RotateCmd

// ----------------------------------------------------------------------------
void RotateCmd::swapZMode(s32 x, s32 y)
{
    Z_MODE = !Z_MODE;
    m_sx = (f32)x; m_sy = (f32)y;
    if (Z_MODE) calcCenter(&m_cx, &m_cy);
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

ScaleCmd::ScaleCmd(list<ISceneNode*> e, s32 x, s32 y) :IObjectCmd(e)
{
    m_sx = (f32) x;
    m_sy = (f32) y;
    m_x =  (f32) x;
    m_y =  (f32) y;
    calcCenter(&m_cx, &m_cy);
} // ScaleCmd

// ----------------------------------------------------------------------------
void ScaleCmd::redo(ISceneNode *node)
{
    f32 s1 = vector2df(m_cx,m_cy).getDistanceFrom(vector2df(m_sx,m_sy));
    f32 s2 = vector2df(m_cx, m_cy).getDistanceFrom(vector2df(m_x, m_y));
    if (s1 < 0.00001f) s1 = 0.00001f;
    if (s2 < 0.00001f) s2 = 0.00001f;
    node->setScale(node->getScale()* s2 / s1);
} // redo

// ----------------------------------------------------------------------------
void ScaleCmd::undo(ISceneNode *node)
{
    f32 s1 = vector2df(m_cx, m_cy).getDistanceFrom(vector2df(m_sx, m_sy));
    f32 s2 = vector2df(m_cx, m_cy).getDistanceFrom(vector2df(m_x, m_y));
    if (s1 < 0.00001f) s1 = 0.00001f;
    if (s2 < 0.00001f) s2 = 0.00001f;
    node->setScale(node->getScale()* s1 / s2);
} // undo

