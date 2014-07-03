#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <irrlicht.h>

#include <list>

using namespace irr;
using namespace scene;
using namespace core;

class ICmd;

// ----------------------------------------------------------------------------
class CommandHandler
{
private:
    std::list<ICmd*>              m_cmd_stack;
    std::list<ICmd*>::iterator    m_it;
public:
    CommandHandler();
    ~CommandHandler();
    void add(ICmd* cmd);
    void redo();
    void undo();
};

#endif
