#ifndef ICOMMAND_HPP
#define ICOMMAND_HPP

#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace core;

// ----------------------------------------------------------------------------
class ICmd
{
public:
    ICmd() {};
    virtual ~ICmd() {};
    virtual void redo() = 0;
    virtual void undo() = 0;
};


#endif
