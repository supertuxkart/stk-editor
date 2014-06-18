#include "road/bezier.hpp"


// ----------------------------------------------------------------------------
float Bezier::bi(int i, float t)
{
    int n = m_cp_num - 1;
    float b;
    b = 1;
    for (int j = 1; j <= i; j++)
        b *= (float)(n - j + 1) / j;
    b *= pow(t, i) * pow(1 - t, n - i);
    return b;
} // bi

// ----------------------------------------------------------------------------
vector3df Bezier::p(float t)
{
    vector3df v(0);
    list<ControlPoint>::Iterator it = m_control_points.begin();

    /*
    t = (t - m_control_points.begin()->t) /
        (m_control_points.getLast()->t - m_control_points.begin()->t);
    */

    for (int i = 0; it != m_control_points.end(); it++, i++)
    {
        v = v + it->pos * bi(i, t);
    }
    return v;
} // p

// ----------------------------------------------------------------------------
vector3df Bezier::getNormal(float t)
{
    vector3df v(0);
    list<ControlPoint>::Iterator it = m_control_points.begin();
    for (int i = 0; it != m_control_points.end(); it++, i++)
    {
        v = v + it->normal * bi(i, t);
    }
    return v;
} // getNormal
