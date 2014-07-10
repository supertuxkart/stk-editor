#ifndef SKY_HPP
#define SKY_HPP

#include "gui/isubscriber.hpp"

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

class Sky :public ISubscriber
{
private:
    ISceneNode* m_sky;

    ITexture* m_up;
    ITexture* m_down;
    ITexture* m_left;
    ITexture* m_right;
    ITexture* m_front;
    ITexture* m_back;

private:
    stringc getTexStr(ITexture* tex);
    void    writeStrc(FILE* fp, stringc str);
    void    readTexSt(FILE* fp, ITexture** tex);

public:
    Sky(ITexture* up,    ITexture* down,  ITexture* left, 
        ITexture* right, ITexture* front, ITexture* back);
    Sky(FILE* fp);

    ~Sky() { m_sky->remove(); }

    void        save(FILE *fp);
    void        notify(ITexture* t);
    stringc     getXmlString();
    ITexture*   getTex(u32 id);


    void        show()         { m_sky->setVisible(true); }
    void        hide()         { m_sky->setVisible(false); }
};

#endif
