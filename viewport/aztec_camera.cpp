#include "viewport/aztec_camera.hpp"

#include "editor.hpp"
#include "input/mouse.hpp"
#include "input/keys.hpp"
#include "viewport/indicator.hpp"

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;

// ----------------------------------------------------------------------------
AztecCamera::AztecCamera(ICameraSceneNode* c, Mouse* m, Keys* k, Indicator* i)
{
    m_cam       = c;
    m_mouse     = m;
    m_keys      = k;
    m_indicator = i;

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

} // AztecCamera

// ----------------------------------------------------------------------------
void AztecCamera::animate(f32 dt)
{
    if (m_keys->state(W_PRESSED) ^ m_keys->state(S_PRESSED))
    {
        float sgn = m_keys->state(S_PRESSED) ? 1.0f : -1.0f;
        vector3df pos = m_cam->getPosition();
        vector3df tar = m_cam->getTarget();
        vector3df transformed_z_dir = getTransformedZdir();

        pos += transformed_z_dir * sgn * dt / 20.0f;
        m_cam->setPosition(pos);

        tar += transformed_z_dir * sgn * dt / 20.0f;
        m_cam->setTarget(tar);

        m_indicator->updatePos(pos, tar);
    };

    if (m_keys->state(A_PRESSED) ^ m_keys->state(D_PRESSED))
    {
        float sgn = m_keys->state(D_PRESSED) ? 1.0f : -1.0f;
        vector3df pos = m_cam->getPosition();
        vector3df tar = m_cam->getTarget();
        vector3df transformed_x_dir = getTransformedXdir();

        pos += transformed_x_dir * sgn * dt / 20.0f;
        m_cam->setPosition(pos);

        tar += transformed_x_dir * sgn * dt / 20.0f;
        m_cam->setTarget(tar);

        m_indicator->updatePos(pos, tar);
    };

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

    if (m_keys->state(SPACE_PRESSED))
    {
        if (m_mouse->left_btn_down)
        {
            vector3df tar = m_cam->getTarget();
            tar.rotateXZBy(m_mouse->dx() / 5.0f, m_cam->getPosition());
            m_cam->setTarget(tar);
            m_indicator->updateTar(tar);
        }

        if (m_mouse->right_btn_down)
        {
            vector3df pos = m_cam->getPosition();
            vector3df tar = m_cam->getTarget();
            vector3df transformed_z_dir = vector3df(pos.X - tar.X, 0, pos.Z - tar.Z);
            transformed_z_dir.normalize();
            tar += transformed_z_dir * (f32)m_mouse->dy();
            m_cam->setTarget(tar);
            m_indicator->updateTar(tar);
        }

        m_mouse->setStorePoint();
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


