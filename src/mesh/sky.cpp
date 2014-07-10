
#include "mesh/sky.hpp"

#include "editor.hpp"

// ----------------------------------------------------------------------------
stringc Sky::getTexStr(ITexture* tex)
{
    stringc h;
    u32 ix;
    h = tex->getName();
    ix = h.findLast('/');
    h = h.subString(ix + 1, h.size() - ix - 1);
    return h;
} // getTexStr

void Sky::writeStrc(FILE* fp, stringc str)
{
    u8 size;
    size = str.size() + 1;
    fwrite(&size, sizeof(u8), 1, fp);
    fwrite(str.c_str(), sizeof(c8), size, fp);
} // writeStrc

void Sky::readTexSt(FILE* fp, ITexture** tex)
{
    u8 size;
    fread(&size, sizeof(u8), 1, fp);
    c8* cc = new c8[size];
    fread(cc, sizeof(c8), size, fp);
    path p = cc;
    *tex = Editor::loadImg(p);
    delete cc;
} // readTexSt

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
    readTexSt(fp, &m_up);
    readTexSt(fp, &m_down);
    readTexSt(fp, &m_left);
    readTexSt(fp, &m_right);
    readTexSt(fp, &m_front);
    readTexSt(fp, &m_back);

    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    m_sky = sm->addSkyBoxSceneNode(m_up, m_down, m_left, m_right, m_front, m_back);
    m_sky->setVisible(false);
} // Sky

// ----------------------------------------------------------------------------
void Sky::save(FILE *fp)
{
    writeStrc(fp, getTexStr(m_up));
    writeStrc(fp, getTexStr(m_down));
    writeStrc(fp, getTexStr(m_left));
    writeStrc(fp, getTexStr(m_right));
    writeStrc(fp, getTexStr(m_front));
    writeStrc(fp, getTexStr(m_back));

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
    u32 ix;

    s += getTexStr(m_up) + " " + getTexStr(m_down) + " " + getTexStr(m_left) + " ";
    s += getTexStr(m_right) + " " + getTexStr(m_front) + " " + getTexStr(m_back);
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
