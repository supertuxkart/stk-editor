
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
    m_sky->setVisible(false);
} // Sky

// ----------------------------------------------------------------------------
Sky::Sky(FILE* fp)
{
    Editor::readTexSt(fp, &m_up);
    Editor::readTexSt(fp, &m_down);
    Editor::readTexSt(fp, &m_left);
    Editor::readTexSt(fp, &m_right);
    Editor::readTexSt(fp, &m_front);
    Editor::readTexSt(fp, &m_back);

    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    m_sky = sm->addSkyBoxSceneNode(m_up, m_down, m_left, m_right, m_front, m_back);
    m_sky->setVisible(false);
} // Sky

// ----------------------------------------------------------------------------
void Sky::save(FILE *fp)
{
    Editor::writeStrc(fp, Editor::getTexStr(m_up));
    Editor::writeStrc(fp, Editor::getTexStr(m_down));
    Editor::writeStrc(fp, Editor::getTexStr(m_left));
    Editor::writeStrc(fp, Editor::getTexStr(m_right));
    Editor::writeStrc(fp, Editor::getTexStr(m_front));
    Editor::writeStrc(fp, Editor::getTexStr(m_back));

} // build

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

    s += Editor::getTexStr(m_up) + " " + Editor::getTexStr(m_down) + " " + 
         Editor::getTexStr(m_right) + " ";
    s += Editor::getTexStr(m_left) + " " + Editor::getTexStr(m_back) + " " + 
         Editor::getTexStr(m_front);
    s += h + "\"/>\n";

    return s;

} // getXmlString

ITexture* Sky::getTex(u32 id)
{
    switch (id)
    {
    case 1:
        return m_up;
    case 2:
        return m_down;
    case 3:
        return m_left;
    case 4:
        return m_right;
    case 5:
        return m_front;
    case 6:
        return m_back;
    default:
        return 0;
    }
}
