#include "viewport/aztec_camera.hpp"
#include "viewport/indicator.hpp"

#include "input/mouse.hpp"
#include "input/keys.hpp"

#include "editor.hpp"

#include <irrlicht.h>

#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;


// ----------------------------------------------------------------------------
void AztecCamera::setOrientation(vector3df dir, vector3df up)
{
    dir *= (m_cam->getPosition() - m_cam->getTarget()).getLength();
    m_cam->setPosition(m_cam->getTarget() - dir);
    m_cam->setUpVector(up);
    m_indicator->update(m_cam->getPosition(), m_cam->getTarget(), up);
} // setOrientation

// ----------------------------------------------------------------------------
void AztecCamera::setHeight(bool new_indi)
{
    dimension2du ss = Editor::getEditor()->getScreenSize();
    matrix4 mat;
    f32 nv = m_cam->getNearValue();
    f32 fv = m_cam->getFarValue();
    f32 hVol = m_normal_cd * ss.Height / ss.Width;
    mat.buildProjectionMatrixOrthoLH(m_normal_cd, hVol, nv, fv);
    m_cam->setProjectionMatrix(mat, true);

    if (!new_indi) m_indicator->setProjMat(m_normal_cd, hVol, nv, fv);
    else m_indicator = new Indicator(m_cam->getPosition(), m_cam->getTarget(), 
                             m_cam->getUpVector(), m_normal_cd, hVol, nv, fv);
} // setHeight

// ----------------------------------------------------------------------------
AztecCamera::AztecCamera(ICameraSceneNode* c, Mouse* m, Keys* k)
{
    m_cx        = 0;
    m_cz        = 0;
    m_cam       = c;
    m_mouse     = m;
    m_keys      = k;
    m_mmb_move  = false;

    m_normal_cd = 250.0f;
    setHeight(true);
    init(0, 0);

    m_iscm = Editor::getEditor()->getSceneManager()->getSceneCollisionManager();
} // AztecCamera

// ----------------------------------------------------------------------------
void AztecCamera::processKeys(f32 dt)
{
    if (m_keys->state(CTRL_PRESSED) || m_keys->state(SHIFT_PRESSED)
        || (m_mmb_move && m_mouse->middle_btn_down)) return;
    // camera moving

    if ((m_keys->state(W_PRESSED) ^ m_keys->state(S_PRESSED))
        || (m_keys->state(A_PRESSED) ^ m_keys->state(D_PRESSED)))
    {
        bool b = false;
        float sgn;
        vector3df pos = m_cam->getPosition();
        vector3df tar = m_cam->getTarget();
        vector3df up  = m_cam->getUpVector();
        up.normalize();
        vector3df d = vector3df(0, 0, 0);
        vector3df n = tar - pos;
        vector3df r = n.crossProduct(up);

        if (m_keys->state(W_PRESSED) ^ m_keys->state(S_PRESSED))
        {
            sgn = m_keys->state(S_PRESSED) ? -1.0f : 1.0f;
            d += up * sgn * dt / 20.0f;
            b = true;
        } // w-s component
        if (m_keys->state(A_PRESSED) ^ m_keys->state(D_PRESSED))
        {
            sgn = m_keys->state(D_PRESSED) ? -1.0f : 1.0f;
            d += r.normalize() * sgn * dt / 20.0f;
            b = true;
        } // a-d component
        if (b)
        {
            pos += d;
            tar += d;
            m_cam->setPosition(pos);
            m_cam->setTarget(tar);
            m_indicator->update(pos, tar, up);
        } // moving
    } // wasd pressed

    // FIX CAMERA STATES
    if (m_keys->m_key_state[NUM_2_PRESSED])
        setOrientation(vector3df(0, 0, -1));
    if (m_keys->m_key_state[NUM_8_PRESSED])
        setOrientation(vector3df(0, 0, 1));
    if (m_keys->m_key_state[NUM_6_PRESSED])
        setOrientation(vector3df(-1, 0, 0));
    if (m_keys->m_key_state[NUM_4_PRESSED])
        setOrientation(vector3df(1, 0, 0));
    if (m_keys->m_key_state[NUM_5_PRESSED])
        setOrientation(vector3df(0,-1,0), vector3df(0,0,-1));
    if (m_keys->m_key_state[NUM_0_PRESSED])
        setOrientation(vector3df(0,1,0), vector3df(0, 0,1));

    if (m_keys->m_key_state[NUM_1_PRESSED])
    {
        restore();
    }
} // processKeys

// ----------------------------------------------------------------------------
void AztecCamera::processMouse(f32 dt)
{
    if (m_mouse->middlePressed())
        m_mmb_move = m_keys->m_key_state[SHIFT_PRESSED];
    if (m_mouse->middleReleased())
        m_mmb_move = false;

    if (m_mmb_move)
    {
        moveWithMouse();
        return;
    }

    // zoom
    if (m_mouse->wheel != 0)
    {
        dimension2du ss = Editor::getEditor()->getScreenSize();
        m_normal_cd -= m_mouse->wheel * 10.0f;
        if (m_normal_cd < 8) m_normal_cd = 8;
        setHeight();
        m_mouse->wheel = 0;
    } // zoom
    bool ss = false;
    // rotation around y
    if ((m_mouse->left_btn_down && m_keys->state(SPACE_PRESSED))
        || (m_mouse->middle_btn_down && !m_mmb_move))
    {
        vector3df pos = m_cam->getPosition();
        vector3df tar = m_cam->getTarget();
        vector3df up  = m_cam->getUpVector();
        pos.rotateXZBy(-m_mouse->dx() / 5.0f, tar);
        up.rotateXZBy(-m_mouse->dx() / 5.0f, vector3df(0,0,0));
        m_cam->setPosition(pos);
        m_indicator->update(pos, tar, up);
        ss = true;
        m_cam->setUpVector(up.normalize());
    } // rot y
    // rotation around transformed x
    if ((m_mouse->right_btn_down && m_keys->state(SPACE_PRESSED))
        || m_mouse->middle_btn_down)
    {
        vector3df pos = m_cam->getPosition();
        vector3df tar = m_cam->getTarget();
        vector3df up  = m_cam->getUpVector();
        vector3df transformed_z_dir;
        vector3df rot;
        transformed_z_dir = (pos - tar).normalize();
        rot = transformed_z_dir.crossProduct(up).normalize();
        quaternion quat;
        quat.fromAngleAxis(degToRad(m_mouse->dy() / 5.0f), rot);
        pos -= tar;
        quat.getMatrix().rotateVect(pos);
        quat.getMatrix().rotateVect(up);
        ss = true;
        pos += tar;
        m_cam->setPosition(pos);
        m_indicator->update(pos, tar, up);
        m_cam->setUpVector(up);
    } // rot transformed x
    if (ss) m_mouse->setStorePoint();
} // processMouse

// ----------------------------------------------------------------------------
void AztecCamera::moveWithMouse()
{
    line3df last_ray = 
        m_iscm->getRayFromScreenCoordinates(vector2d<s32>(m_mouse->prev_x, 
                                                            m_mouse->prev_y));
    line3df ray = 
        m_iscm->getRayFromScreenCoordinates(vector2d<s32>(m_mouse->x, 
                                                            m_mouse->y));
    vector3df d = (ray.start - last_ray.start);
    vector3df pos = m_cam->getPosition();
    vector3df tar = m_cam->getTarget();
    pos -= d;
    tar -= d;
    m_cam->setPosition(pos);
    m_cam->setTarget(tar);
    m_indicator->update(pos, tar, m_cam->getUpVector());
    m_mouse->setStorePoint();
} // moveWithMouse

// ----------------------------------------------------------------------------
void AztecCamera::animate(f32 dt)
{
    processMouse(dt);
    processKeys(dt);
} // animate

// ----------------------------------------------------------------------------
vector3df AztecCamera::getTransformedXdir()
{
    vector3df transformed_z_dir;
    vector3df up;
    vector3df pos = m_cam->getPosition();
    vector3df tar = m_cam->getTarget();
    transformed_z_dir = (pos-tar).normalize();
    up = m_cam->getUpVector();
    return up.crossProduct(transformed_z_dir).normalize();
} // getTransformedXdir

// ----------------------------------------------------------------------------
vector3df AztecCamera::getTransformedYdir()
{
    vector3df pos = m_cam->getPosition();
    vector3df transformed_y_dir = m_cam->getUpVector();
    transformed_y_dir.normalize();
    return transformed_y_dir;
} // getTransformedYdir

// ----------------------------------------------------------------------------
vector3df AztecCamera::getTransformedZdir()
{
    vector3df pos = m_cam->getPosition();
    vector3df tar = m_cam->getTarget();
    vector3df transformed_z_dir = (pos - tar).normalize();
    return transformed_z_dir;
} // getTransformedZdir

// ----------------------------------------------------------------------------
void AztecCamera::init(f32 x, f32 z)
{
    m_cx = x / 2.0f;
    m_cz = z / 2.0f;
    m_cam->setPosition(vector3df(m_cx - 15, 80, m_cz));
    m_cam->setTarget(vector3df(m_cx - 15, 0, m_cz));
    m_cam->setUpVector(vector3df(0, 0, 1));
    m_indicator->update(vector3df(m_cx, 80, m_cz + 20), 
          vector3df(m_cx, 0, m_cz), vector3df(0, 0, 1));
} // init

// ----------------------------------------------------------------------------
void AztecCamera::restore()
{
    m_normal_cd = 250.0f;
    setHeight();
    init(m_cx * 2, m_cz * 2);
} // restore
