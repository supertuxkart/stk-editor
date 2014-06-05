#include "command.hpp"

// ----------------------------------------------------------------------------
// Command  -------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Command::redo()
{
    std::list<Entity*>::iterator it;
    for (it = m_elements.begin(); it != m_elements.end(); it++)
        redo((*it));
} // redo

// ----------------------------------------------------------------------------
void Command::undo()
{
    std::list<Entity*>::iterator it;
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
