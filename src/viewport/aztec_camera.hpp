#ifndef AZTEC_CAMERA_HPP
#define AZTEC_CAMERA_HPP

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;

struct Mouse;
struct Keys;
class  Indicator;

class AztecCamera
{
private:
    ICameraSceneNode*       m_cam;

    Mouse*                  m_mouse;
    Keys*                   m_keys;

    Indicator*              m_indicator;

    f32                     m_normal_cd;

public:
    AztecCamera(ICameraSceneNode* c, Mouse* m, Keys* k);
    void animate(f32 dt);

    vector3df               getTransformedXdir();
    vector3df               getTransformedZdir();

    ICameraSceneNode*       Cam()                    { return m_cam;       }
    Indicator*              getIndicator()           { return m_indicator; }
};

#endif

