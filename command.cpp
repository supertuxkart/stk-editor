#include "command.hpp"

// ----------------------------------------------------------------------------
// Command  -------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Command::redo()
{
    std::list<ISceneNode*>::iterator it;
    for (it = m_elements.begin(); it != m_elements.end(); it++)
        redo((*it));
} // redo

// ----------------------------------------------------------------------------
void Command::undo()
{
    std::list<ISceneNode*>::iterator it;
    for (it = m_elements.begin(); it != m_elements.end(); it++)
        undo((*it));
} // undo


// ----------------------------------------------------------------------------
// This function will set the two variable with smaller absolout value to 0
void Command::limit(float& a, float& b, float& c)
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
// CommandHandler  ------------------------------------------------------------
// ----------------------------------------------------------------------------
CommandHandler::CommandHandler()
{
    m_it = m_cmd_stack.begin();
}

// ----------------------------------------------------------------------------
void CommandHandler::add(Command* cmd)
{
    std::list<Command*>::iterator it;
    
    for (it = m_it; it != m_cmd_stack.end(); it++) delete (*it);
    m_it = m_cmd_stack.erase(m_it, it);
    
    m_it = m_cmd_stack.insert(m_it, cmd);
    m_it++;

} // add

// ----------------------------------------------------------------------------
void CommandHandler::redo()
{
    if (m_it != m_cmd_stack.end())
    {
        (*m_it)->redo();
        m_it++;
    }
} // redo

// ----------------------------------------------------------------------------
void CommandHandler::undo()
{
    if (m_it != m_cmd_stack.begin())
    {
        m_it--;
        (*m_it)->undo();
    }
} // undo


// ----------------------------------------------------------------------------
// DelCmd  --------------------------------------------------------------------
// ----------------------------------------------------------------------------
DelCmd::~DelCmd()
{
    if (m_rdy)
    {
        std::list<ISceneNode*>::iterator it;
        for (it = m_elements.begin(); it != m_elements.end(); it++)
        {
            (*it)->remove();
        }
    }
} // ~DelCmd


// ----------------------------------------------------------------------------
// MoveCmd  -------------------------------------------------------------------
// ----------------------------------------------------------------------------

MoveCmd::MoveCmd(std::list<ISceneNode*> e, bool limited) :Command(e) 
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

RotateCmd::RotateCmd(std::list<ISceneNode*> e, bool limited) :Command(e) 
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

ScaleCmd::ScaleCmd(std::list<ISceneNode*> e, bool limited) :Command(e)
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

