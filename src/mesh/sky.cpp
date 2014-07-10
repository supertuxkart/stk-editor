
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
    bool b = m_sky->isVisible();
    switch (m_id)
    {
    case 1:
        m_up = t;
        break;
    case 2:
        m_down = t;
        break;
    case 3:
        m_left = t;
        break;
    case 4:
        m_right = t;
        break;
    case 5:
        m_front = t;
        break;
    case 6:
        m_back = t;
        break;
    default:
        break;
    }
    m_sky->remove();
    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    m_sky = sm->addSkyBoxSceneNode(m_up, m_down, m_left, m_right, m_front, m_back);
    m_sky->setVisible(b);
} // notify

// ----------------------------------------------------------------------------
stringc Sky::getXmlString()
{
    stringc s = "  <sky-box texture=\"";
    stringc h;
    u32 ix;

    h = m_up->getName();    ix = h.findLast('/'); h = h.subString(ix + 1, h.size() - ix - 1);
    s += h + " ";
    h = m_down->getName();  ix = h.findLast('/'); h = h.subString(ix + 1, h.size() - ix - 1);
    s += h + " ";
    h = m_left->getName();  ix = h.findLast('/'); h = h.subString(ix + 1, h.size() - ix - 1);
    s += h + " "; 
    h = m_right->getName(); ix = h.findLast('/'); h = h.subString(ix + 1, h.size() - ix - 1);
    s += h + " "; 
    h = m_front->getName(); ix = h.findLast('/'); h = h.subString(ix + 1, h.size() - ix - 1);
    s += h + " "; 
    h = m_back->getName();  ix = h.findLast('/'); h = h.subString(ix + 1, h.size() - ix - 1);
    s += h + "\"/>\n";

    return s;

} // getXmlString

