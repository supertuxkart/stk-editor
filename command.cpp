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
void MoveCmd::redo(ISceneNode *node)
{
    node->setPosition(node->getPosition() + vector3df(m_dx, m_dy, m_dz));
} // redo

// ----------------------------------------------------------------------------
void MoveCmd::undo(ISceneNode *node)
{
    node->setPosition(node->getPosition() + vector3df(-m_dx, -m_dy, -m_dz));
} // undo


// ----------------------------------------------------------------------------
// RotateCmd  -----------------------------------------------------------------
// ----------------------------------------------------------------------------
void RotateCmd::redo(ISceneNode *node)
{
    node->setRotation(node->getRotation() + vector3df(m_dx, m_dy, m_dz));
} // redo

// ----------------------------------------------------------------------------
void RotateCmd::undo(ISceneNode *node)
{
    node->setRotation(node->getRotation() + vector3df(-m_dx, -m_dy, -m_dz));
} // undo


