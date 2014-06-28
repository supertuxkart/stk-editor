#ifndef ROADCOMMAND_HPP
#define ROADCOMMAND_HPP

#include "commands/icommand.hpp"


#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace core;


class ISpline;
class IRoad;

// ----------------------------------------------------------------------------
class RoadCommand :public ICommand
{
private:
    IRoad*          m_road;
    ISpline*        m_spline;
    vector3df       m_pos;
    bool            m_insert;
    bool            m_created;
    u32             m_ix;
public:
    RoadCommand(IRoad* road, bool insert);
    virtual ~RoadCommand() {};
    void updatePos(vector3df pos);
    void redo();
    void undo();
};

#endif

