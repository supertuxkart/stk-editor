#ifndef DRIVELINE_HPP
#define DRIVELINE_HPP

#include "mesh/iroad.hpp"

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace io;

class DriveLine :public IRoad
{
private:
    
    Mesh            m_mesh;
    SMaterial       m_material;
    bool order(vector3df* v1, vector3df* v2, vector3df* v3, vector3df* v4, vector3df n);

    virtual void    textureExport(FILE* fp) {};

public:
    DriveLine(ISceneNode* parent, ISceneManager* mgr, s32 id, ISpline* s, stringw n);
    DriveLine(ISceneNode* parent, ISceneManager* mgr, s32 id, FILE* fp);

    void    refresh();
    void    build(path p);

    void    render();
};

#endif
