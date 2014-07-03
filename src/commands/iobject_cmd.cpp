#include "commands/iobject_cmd.hpp"

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
// This function will set the two variable with smaller absolout value to 0
void IObjectCmd::limit(float& a, float& b, float& c)
{
    if (a*a > b*b && a*a > c*c)
    {
        b = 0;
        c = 0;
    }
    else
    {
        a = 0;
        if (b*b > c*c) c = 0;
        else b = 0;
    }
}


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

MoveCmd::MoveCmd(list<ISceneNode*> e, bool limited) :IObjectCmd(e)
{
    m_dx = 0; m_dy = 0; m_dz = 0;
    m_limited = limited;
}

// ----------------------------------------------------------------------------
void MoveCmd::redo(ISceneNode *node)
{
    float dx, dy, dz;
    dx = m_dx; dy = m_dy; dz = m_dz;
    if (m_limited) limit(dx, dy, dz);
    node->setPosition(node->getPosition() + vector3df(dx / 10.0f, dy / 10.0f, dz / 10.0f));
} // redo

// ----------------------------------------------------------------------------
void MoveCmd::undo(ISceneNode *node)
{
    float dx, dy, dz;
    dx = m_dx; dy = m_dy; dz = m_dz;
    if (m_limited) limit(dx, dy, dz);
    node->setPosition(node->getPosition() + vector3df(-dx / 10.0f, -dy / 10.0f, -dz / 10.0f));
} // undo

// ----------------------------------------------------------------------------
// RotateCmd  -----------------------------------------------------------------
// ----------------------------------------------------------------------------

RotateCmd::RotateCmd(list<ISceneNode*> e, bool limited) :IObjectCmd(e)
{
    m_dx = 0; m_dy = 0; m_dz = 0;
    m_limited = limited;
}

// ----------------------------------------------------------------------------
void RotateCmd::redo(ISceneNode *node)
{
    float dx, dy, dz;
    dx = m_dx; dy = m_dy; dz = m_dz;
    if (m_limited) limit(dx, dy, dz);
    node->setRotation(node->getRotation() + vector3df(dz, dy, dx));
} // redo

// ----------------------------------------------------------------------------
void RotateCmd::undo(ISceneNode *node)
{
    float dx, dy, dz;
    dx = m_dx; dy = m_dy; dz = m_dz;
    if (m_limited) limit(dx, dy, dz);
    node->setRotation(node->getRotation() + vector3df(-dz, -dy, -dx));
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
    limit(dx, dy, dz);
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
    limit(dx, dy, dz);
    if (m_limited)
    {
        dx = dx + dy + dz;
        dy = dx;
        dz = dx;
    }
    node->setScale(node->getScale() + vector3df(-dz / 10.0f, -dy / 10.0f, -dx/ 10.0f));
} // undo

