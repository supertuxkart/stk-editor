#ifndef WELCOME_SCREEN_HPP
#define WELCOME_SCREEN_HPP

#include <irrlicht.h>

using namespace irr;
using namespace gui;
using namespace core;

class WelcomeScreen
{
public:
    static const int FBTN_ID = 550;
private:

    // private variables:
    static WelcomeScreen*   m_self;

    IGUIWindow*             m_wndw;


    // private functions:

    void    init();

    WelcomeScreen() {};

protected:
    void                initWndw(IGUIWindow* wndw);

public:
    static WelcomeScreen*   get();
    u32                     getSize();

    void                    hide()              { m_wndw->setVisible(false); }
    void                    show()              { m_wndw->setVisible(true);  }

};

#endif
