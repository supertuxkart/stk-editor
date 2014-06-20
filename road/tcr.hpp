#ifndef TCR_HPP
#define TCR_HPP

#include "road/ispline.hpp"

class TCR :public ISpline
{
private:
    vector3df ai2(vector3df p0, vector3df p1, vector3df v0, vector3df v1, 
                  list<ControlPoint>::Iterator& it);
    vector3df ai3(vector3df p0, vector3df p1, vector3df v0, vector3df v1, 
                  list<ControlPoint>::Iterator& it);

    void      calcWeight(list<ControlPoint>::Iterator& it, float tx,
                         float* wm, float* w0, float* w1, float* w2);


public:
    TCR(ISceneNode* parent, ISceneManager* mgr, s32 id) :ISpline(parent, mgr, id) {};
    vector3df p(float t);
    vector3df getNormal(float t);
    float getWidth(float t);
};

#endif
