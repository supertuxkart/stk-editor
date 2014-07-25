#ifndef INDICATOR_HPP
#define INDICATOR_HPP

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

class Indicator
{
private:
    IVideoDriver*       m_vd;
    ISceneManager*      m_sm;
    ICameraSceneNode*   m_cam;
    ITexture*           m_rtt;
    ISceneNode*         m_model;
    u32 static const    m_offset = 3333;
    dimension2du        m_ss;
public:
    Indicator(vector3df p, vector3df t, vector3df u,
              f32 wVol, f32 hWol, f32 zNear, f32 zFar);
    ~Indicator();

    void        update(vector3df pos, vector3df tar, vector3df up);
    void        setProjMat(f32 wVol, f32 hWol, f32 zNear, f32 zFar);
    void        renderToTexture();
    void        drawToScreen();
    void        reallocate();

    ITexture*   getTexture()             { return m_rtt; }

};



#endif
