#include "viewport/indicator.hpp"

#include "editor.hpp"


// ----------------------------------------------------------------------------
Indicator::Indicator(vector3df p, vector3df t, f32 wVol, f32 hWol, f32 zNear, f32 zFar)
{
    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    IVideoDriver*  vd = Editor::getEditor()->getVideoDriver();

    m_cam = sm->addCameraSceneNode();
    updatePos(p, t);
    setProjMat(wVol, hWol, zNear, zFar);
    
    m_model = sm->addAnimatedMeshSceneNode(
        sm->getMesh(L"img/indicator.b3d"), m_cam);

    m_ss = vd->getScreenSize();
    m_rtt = vd->addRenderTargetTexture(m_ss, "RTT1", ECF_A8R8G8B8);
    m_vd = vd;
    m_sm = sm;
} // Indicator

// ----------------------------------------------------------------------------
Indicator::~Indicator()
{
} // ~indicator

// ----------------------------------------------------------------------------
void Indicator::updatePos(vector3df pos, vector3df tar)
{
    pos.X += m_offset;
    tar.X += m_offset;
    m_cam->setPosition(pos);
    m_cam->updateAbsolutePosition();
    m_cam->setTarget(tar);
} // updatePos

// ----------------------------------------------------------------------------
void Indicator::setProjMat(f32 wVol, f32 hWol, f32 zNear, f32 zFar)
{
    matrix4 mat;
    mat.buildProjectionMatrixOrthoLH(wVol, hWol, zNear, zFar);
    m_cam->setProjectionMatrix(mat, true);

} // setProjMat

// ----------------------------------------------------------------------------
void Indicator::renderToTexture()
{
    m_vd->setRenderTarget(m_rtt, true, true);
    
    ICameraSceneNode* cam = m_sm->getActiveCamera();
    m_sm->setActiveCamera(m_cam);

    m_sm->drawAll();

    m_vd->setRenderTarget(0, true, true, SColor(255, 80, 0, 170));
    m_sm->setActiveCamera(cam);

} // render

// ----------------------------------------------------------------------------
void Indicator::drawToScreen()
{
    u32 mx = m_ss.Width / 10;
    rect<s32> indiFrame(m_ss.Width / 2 - mx, m_ss.Height / 2 - mx,
        m_ss.Width / 2 + mx, m_ss.Height / 2 + mx);
    m_vd->draw2DImage(m_rtt, position2d<s32>(0, m_ss.Height - 2 * mx), indiFrame,
        (rect<s32>*)0, SColor(255, 255, 255, 255), true);
} // drawToScreen

// ----------------------------------------------------------------------------
void Indicator::reallocate()
{
    m_ss = m_vd->getScreenSize();
    m_vd->removeTexture(m_rtt);
    m_rtt = m_vd->addRenderTargetTexture(m_ss, "RTT1", ECF_A8R8G8B8);
} // reallocate
