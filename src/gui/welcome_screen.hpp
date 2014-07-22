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
    IGUIListBox*            m_lb;

    // private functions:

    void    init();

    WelcomeScreen() {};
public:
    static WelcomeScreen*   get();
    u32                     getSize();
    stringc                 getActiveText();

    void                    hide()              { m_wndw->setVisible(false); }
    void                    show()              { m_wndw->setVisible(true);  }

};

#endif
