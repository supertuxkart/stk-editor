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
    m_rtt = vd->addRenderTargetTexture(m_ss,"RTT1",ECF_A8R8G8B8);
    m_vd = vd;
    m_sm = sm;
} // Indicator

// ----------------------------------------------------------------------------
Indicator::~Indicator()
{
    m_cam->drop();
    m_model->drop();
} // ~indicator

// ----------------------------------------------------------------------------
void Indicator::updatePos(vector3df pos, vector3df tar)
{
    pos.X += m_offset;
    tar.X += m_offset;
    m_cam->setPosition(pos);
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
void Indicator::render()
{
    m_vd->setRenderTarget(m_rtt, true, true);
    
    ICameraSceneNode* cam = m_sm->getActiveCamera();
    m_sm->setActiveCamera(m_cam);

    m_sm->drawAll();

    m_vd->setRenderTarget(0, true, true, SColor(255, 80, 0, 170));
    m_sm->setActiveCamera(cam);

    rect<s32> indiFrame(m_ss.Width / 2 - 110, m_ss.Height / 2 - 110,
                        m_ss.Width / 2 + 110, m_ss.Height / 2 + 110);
    m_vd->draw2DImage(m_rtt, position2d<s32>(0, m_ss.Height - 220), indiFrame, 
                      (rect<s32>*)0, SColor(255, 255, 255, 255), true);

} // render

