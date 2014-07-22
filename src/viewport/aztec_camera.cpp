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
AztecCamera::AztecCamera(ICameraSceneNode* c, Mouse* m, Keys* k)
{
    m_x         = 0;
    m_z         = 0;
    m_cam       = c;
    m_mouse     = m;
    m_keys      = k;

    m_normal_cd = 50.0f;

    dimension2du ss = Editor::getEditor()->getScreenSize();
    matrix4 mat;
    f32 nv = m_cam->getNearValue();
    f32 fv = m_cam->getFarValue();
    f32 hVol = m_normal_cd * ss.Height / ss.Width;
    mat.buildProjectionMatrixOrthoLH(m_normal_cd, hVol, nv, fv);
    m_cam->setProjectionMatrix(mat, true);
    m_indicator = new Indicator(m_cam->getPosition(),
                                m_cam->getTarget(),m_normal_cd, hVol, nv, fv);
    init(0, 0);
} // AztecCamera

// ----------------------------------------------------------------------------
void AztecCamera::animate(f32 dt)
{
    if (m_mouse->wheel != 0)
    {
        dimension2du ss = Editor::getEditor()->getScreenSize();
        m_normal_cd -= m_mouse->wheel * 10.0f;
        if (m_normal_cd < 8) m_normal_cd = 8;
        matrix4 mat;

        f32 nv = m_cam->getNearValue();
        f32 fv = m_cam->getFarValue();
        f32 hVol = m_normal_cd * ss.Height / ss.Width;

        mat.buildProjectionMatrixOrthoLH(m_normal_cd, hVol, nv, fv);
        m_cam->setProjectionMatrix(mat, true);
        m_mouse->wheel = 0;

        m_indicator->setProjMat(m_normal_cd, hVol, nv, fv);
    }
    
    bool ss = false;

    if ((m_mouse->left_btn_down && m_keys->state(SPACE_PRESSED))
                                  || m_mouse->middle_btn_down)
    {
        vector3df pos = m_cam->getPosition();
        vector3df tar = m_cam->getTarget();
        tar.rotateXZBy(- m_mouse->dx() / 5.0f, vector3df(m_x, 0, m_z));
        pos.rotateXZBy(- m_mouse->dx() / 5.0f, vector3df(m_x, 0, m_z));
        m_up_p.rotateXZBy(- m_mouse->dx() / 5.0f, vector3df(m_x, 0, m_z));
        m_cam->setPosition(pos);
        m_cam->updateAbsolutePosition();
        m_cam->setTarget(tar);
        m_indicator->updatePos(pos,tar);
        ss = true;
    }

    if ((m_mouse->right_btn_down && m_keys->state(SPACE_PRESSED))
        || m_mouse->middle_btn_down)
    {
        vector3df pos = m_cam->getPosition();
        vector3df tar = m_cam->getTarget();
        vector3df transformed_z_dir;
        vector3df rot;
        transformed_z_dir = vector3df(pos.X - tar.X, 0, pos.Z - tar.Z).normalize();
        rot = transformed_z_dir.crossProduct(vector3df(0, 1, 0)).normalize();
        quaternion quat;
        quat.fromAngleAxis(degToRad(m_mouse->dy() / 5.0f), rot);
        tar -= vector3df(m_x, m_y, m_z);
        pos -= vector3df(m_x, m_y, m_z);
        vector3df up_p = m_up_p;
        up_p -= vector3df(m_x, m_y, m_z);
        quat.getMatrix().rotateVect(tar);
        quat.getMatrix().rotateVect(pos);
        quat.getMatrix().rotateVect(up_p);
        ss = true;
        if (fabs(tar.X - pos.X) + fabs(tar.Z - pos.Z) > 20)
        {
            tar += vector3df(m_x, m_y, m_z);
            pos += vector3df(m_x, m_y, m_z);            
            m_cam->setPosition(pos);
            m_cam->updateAbsolutePosition();
            m_cam->setTarget(tar);
            m_indicator->updatePos(pos, tar);
            m_up_p = up_p + vector3df(m_x, m_y, m_z);
        }
    }
    if (ss) m_mouse->setStorePoint();

    if (m_keys->state(CTRL_PRESSED) || m_keys->state(SHIFT_PRESSED)) return;
    
    if ((m_keys->state(W_PRESSED) ^ m_keys->state(S_PRESSED))
        || (m_keys->state(A_PRESSED) ^ m_keys->state(D_PRESSED)))
    {
        bool b = false;
        float sgn;
        vector3df pos = m_cam->getPosition();
        vector3df tar = m_cam->getTarget();
        vector3df d = vector3df(0, 0, 0);
        vector3df n = vector3df(m_x,m_y,m_z) - pos;
        vector3df r = n.crossProduct(m_up_p-pos);

        if (m_keys->state(W_PRESSED) ^ m_keys->state(S_PRESSED))
        {
            sgn = m_keys->state(S_PRESSED) ? 1.0f : -1.0f;
            d += vector3df(m_up_p - pos).normalize() * sgn * dt / 20.0f;
            b = true;
        }
        if (m_keys->state(A_PRESSED) ^ m_keys->state(D_PRESSED))
        {
            sgn = m_keys->state(D_PRESSED) ? 1.0f : -1.0f;
            d += r.normalize() * sgn * dt / 20.0f;
            b = true;
        }
        if (b)
        {
            m_up_p += d;
            m_x += d.X; m_y += d.Y; m_z += d.Z;
            pos += d;
            m_cam->setPosition(pos);
            m_cam->updateAbsolutePosition();
            tar += d;
            m_cam->setTarget(tar);
            m_indicator->updatePos(pos, tar);
        }
    }

} // animate

// ----------------------------------------------------------------------------
vector3df AztecCamera::getTransformedXdir()
{
    vector3df transformed_z_dir = getTransformedZdir();
    transformed_z_dir.rotateXZBy(90);

    return transformed_z_dir;

} // getTransformedXdir

// ----------------------------------------------------------------------------
vector3df AztecCamera::getTransformedZdir()
{
    vector3df pos = m_cam->getPosition();
    vector3df tar = m_cam->getTarget();
    vector3df transformed_z_dir = vector3df(pos.X - tar.X, 0, pos.Z - tar.Z);
    transformed_z_dir.normalize();
    return transformed_z_dir;
} // getTransformedZdir

// ----------------------------------------------------------------------------
void AztecCamera::init(f32 x, f32 z)
{
    m_x = x / 2.0f;
    m_z = z / 2.0f;
    m_y = 0;
    m_cam->setPosition(vector3df(m_x, 80, m_z + 20));
    m_cam->updateAbsolutePosition();
    m_cam->setTarget(vector3df(m_x, 0, m_z));
    m_up_p = vector3df(m_x, 80, m_z + 22);
} // setOffset
