#include "commands/command_handler.hpp"
#include "commands/icmd.hpp"

// ----------------------------------------------------------------------------
CommandHandler::CommandHandler()
{
    m_it = m_cmd_stack.begin();
} // CommandHandler

CommandHandler::~CommandHandler()
{
    std::list<ICmd*>::iterator it;
    for (it = m_cmd_stack.begin(); it != m_cmd_stack.end(); it++) delete (*it);
} // ~CommandHandler

// ----------------------------------------------------------------------------
void CommandHandler::add(ICmd* cmd)
{
    std::list<ICmd*>::iterator it;

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
void CommandHandler::clear()
{
    std::list<ICmd*>::iterator it;
    for (it = m_cmd_stack.begin(); it != m_cmd_stack.end(); it++) delete (*it);
    m_cmd_stack.clear();
} // undo

