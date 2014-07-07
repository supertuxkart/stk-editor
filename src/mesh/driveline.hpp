#ifndef DRIVELINE_HPP
#define DRIVELINE_HPP

#include "mesh/iroad.hpp"

class DriveLine :public IRoad
{

public:
    DriveLine(ISceneNode* parent, ISceneManager* mgr, s32 id, ISpline* s, stringw n)
        :IRoad(parent, mgr, id, s, n) {};
    DriveLine(ISceneNode* parent, ISceneManager* mgr, s32 id, FILE* fp)
        :IRoad(parent, mgr, id, fp) {};

    void    refresh();
    void    build();

    void    render();
};

#endif
