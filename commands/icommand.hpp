#ifndef ICOMMAND_HPP
#define ICOMMAND_HPP

#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace core;

// ----------------------------------------------------------------------------
class ICommand
{
public:
    ICommand() {};
    virtual ~ICommand() {};
    virtual void redo() = 0;
    virtual void undo() = 0;
};


#endif
