#ifndef BEZIER_HPP
#define BEZIER_HPP

#include "spline/ispline.hpp"

class Bezier :public ISpline
{
private:
    float bi(int i, float t);
public:
    Bezier(ISceneNode* parent, ISceneManager* mgr, s32 id) 
                  :ISpline(parent, mgr, id, L"Bezier") {};
    Bezier(ISceneNode* parent, ISceneManager* mgr, s32 id, FILE* fp)
                  :ISpline(parent, mgr, id, L"Bezier", fp) {};
    vector3df p(float t);
    vector3df getNormal(float t);
    float getWidth(float t);
};

#endif
