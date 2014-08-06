#ifndef RCSCMD_HPP
#define RCSCMD_HPP

#include "commands/icmd.hpp"
#include "mesh/road.hpp"

#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace core;

// ----------------------------------------------------------------------------
class RCSCmd :public ICmd
{
private:
    Road*           m_road;
    array<vector2df> m_b;
    array<vector2df> m_a;
public:
    RCSCmd(Road* r, array<vector2df> b, array<vector2df> a) :m_road(r), m_b(b), m_a(a){};
    virtual ~RCSCmd() {};
    
    void redo() { m_road->setCrossSection(m_a); m_road->refresh(); }
    void undo() { m_road->setCrossSection(m_b); m_road->refresh(); }
};

#endif
