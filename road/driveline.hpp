#ifndef DRIVELINE_HPP
#define DRIVELINE_HPP

#include "road/iroad.hpp"

class DriveLine :public IRoad
{

public:
    DriveLine(ISceneNode* parent, ISceneManager* mgr, s32 id, ISpline* s)
        :IRoad(parent, mgr, id, s) {};

    void    refresh();

    void    render();
};

#endif
