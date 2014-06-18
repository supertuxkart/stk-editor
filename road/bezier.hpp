#ifndef BEZIER_HPP
#define BEZIER_HPP

#include "road/ispline.hpp"

class Bezier :public ISpline
{
private:
    float bi(int i, float t);
public:
    Bezier(ISceneNode* parent, ISceneManager* mgr, s32 id) :ISpline(parent, mgr, id) {};
    vector3df p(float t);
    vector3df getNormal(float t);
};

#endif
