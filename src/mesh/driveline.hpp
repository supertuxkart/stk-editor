#ifndef DRIVELINE_HPP
#define DRIVELINE_HPP

#include "mesh/iroad.hpp"

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace io;

class DriveLine :public IRoad
{

public:
    DriveLine(ISceneNode* parent, ISceneManager* mgr, s32 id, ISpline* s, stringw n)
        :IRoad(parent, mgr, id, s, n) {};
    DriveLine(ISceneNode* parent, ISceneManager* mgr, s32 id, FILE* fp)
        :IRoad(parent, mgr, id, fp) {};

    void    refresh();
    void    build(path p);

    void    render();
};

#endif
