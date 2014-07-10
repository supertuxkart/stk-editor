#include "mesh/sky.hpp"

#include "editor.hpp"

// ----------------------------------------------------------------------------
Sky::Sky(ITexture* up, ITexture*    down, ITexture* left,
         ITexture* right, ITexture* front, ITexture* back)
{
    m_up    = up;
    m_down  = down;
    m_left  = left;
    m_right = right;
    m_front = front;
    m_back  = back;

    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    m_sky = sm->addSkyBoxSceneNode(up, down, left, right, front, back);

} // Sky


// ----------------------------------------------------------------------------
void Sky::notify(ITexture* t)
{
    switch (m_id)
    {
    case 0:
        m_up = t;
        break;
    case 1:
        m_down = t;
        break;
    case 2:
        m_left = t;
        break;
    case 3:
        m_right = t;
        break;
    case 4:
        m_front = t;
        break;
    case 5:
        m_back = t;
        break;
    default:
        break;
    }

    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    m_sky = sm->addSkyBoxSceneNode(m_up, m_down, m_left, m_right, m_front, m_back);

} // notify

