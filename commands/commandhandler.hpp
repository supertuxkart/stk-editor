#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <irrlicht.h>

#include <list>

using namespace irr;
using namespace scene;
using namespace core;

class ICommand;

// ----------------------------------------------------------------------------
class CommandHandler
{
private:
    std::list<ICommand*>              m_cmd_stack;
    std::list<ICommand*>::iterator    m_it;
public:
    CommandHandler();
    ~CommandHandler();
    void add(ICommand* cmd);
    void redo();
    void undo();
};

#endif
