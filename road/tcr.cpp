#include "road/tcr.hpp"

// ----------------------------------------------------------------------------
vector3df TCR::ai2(vector3df p0, vector3df p1, vector3df v0, vector3df v1, 
                  list<ControlPoint>::Iterator& it)
{
    float t0, t1;

    t0 = it->t;
    t1 = (it + 1)->t;

    return (p1 - p0) * 3 / pow(t1-t0, 2) - (v1 + v0 * 2) / (t1-t0);

} // ai2

// ----------------------------------------------------------------------------
vector3df TCR::ai3(vector3df p0, vector3df p1, vector3df v0, vector3df v1, 
                  list<ControlPoint>::Iterator& it)
{
    float t0, t1;
    t0 = it->t;
    t1 = (it + 1)->t;

    return (p0 - p1) * 2 / pow(t1-t0, 3) + (v1 + v0) / pow(t1-t0, 2);

} // ai3

void TCR::calcWeight(list<ControlPoint>::Iterator& it, float tx,
                     float* wm, float* w0, float* w1, float* w2)
{
    vector3df pm(0, 0, 0);
    vector3df p0(1, 0, 0);
    vector3df p1(0, 1, 0);
    vector3df p2(0, 0, 1);

    vector3df v0 = calculateVelInPoint(pm, p0, p1, it);
    vector3df v1 = calculateVelInPoint(p0, p1, p2, it + 1);

    vector3df w = ai3(p0, p1, v0, v1, it) * pow(tx, 3) + ai2(p0, p1, v0, v1, it)
        * pow(tx, 2) + v0 * tx + p0;

    pm = vector3df(1, 0, 0);
    p0 = vector3df(0, 0, 0);
    v0 = calculateVelInPoint(pm, p0, p1, it);

    v1 = calculateVelInPoint(p0, p1, p2, it + 1);

    *w0 = w.X;
    *w1 = w.Y;
    *w2 = w.Z;
    *wm = (ai3(p0, p1, v0, v1, it) * pow(tx, 3) + ai2(p0, p1, v0, v1, it)
        * pow(tx, 2) + v0 * tx + p0).X;
}

// ----------------------------------------------------------------------------
vector3df TCR::p(float t)
{
    if (t > 1.0f) return m_control_points.getLast()->pos;

    list<ControlPoint>::Iterator it = m_control_points.begin();
    while (t > (it + 1)->t) it++;

    vector3df p0, p1;
    vector3df v0, v1;

    p0 = it->pos;
    p1 = (it + 1)->pos;
    v0 = it->vel;
    v1 = (it + 1)->vel;

    float tx = t - it->t;
    return ai3(p0, p1, v0, v1, it) * pow(tx, 3) + ai2(p0, p1, v0, v1, it) 
           * pow(tx, 2) + v0 * tx + p0;
} // p

// ----------------------------------------------------------------------------
vector3df TCR::getNormal(float t)
{
    if (t > 1.0f) t = 1.0f;

    list<ControlPoint>::Iterator it = m_control_points.begin();
    
    while (t > (it + 1)->t) it++;
    float tx = t - it->t;

    float wm, w0, w1, w2;

    calcWeight(it, tx, &wm, &w0, &w1, &w2);

    vector3df norm = it->normal * w0;
    if (it + 1 != m_control_points.end())
    {
        norm += (it + 1)->normal * w1;
        if (it + 2 != m_control_points.end())
            norm += (it + 2)->normal * w2;
    }
    if (it != m_control_points.begin())
    {
        norm += (it - 1)->normal * wm;
    }

    return norm;

} // getNormal

// ----------------------------------------------------------------------------
float TCR::getWidth(float t)
{
    if (t > 1.0f) t = 1.0f;

    list<ControlPoint>::Iterator it = m_control_points.begin();

    while (t > (it + 1)->t) it++;
    float tx = t - it->t;

    float wm, w0, w1, w2;

    calcWeight(it, tx, &wm, &w0, &w1, &w2);

    float f = it->width * w0;
    if (it + 1 != m_control_points.end())
    {
        f += (it + 1)->width * w1;
        if (it + 2 != m_control_points.end())
            f += (it + 2)->width * w2;
    }
    if (it != m_control_points.begin())
    {
        f += (it - 1)->width * wm;
    }

    return f;

} // getWidth
