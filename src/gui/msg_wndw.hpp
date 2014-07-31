#ifndef MSG_WNDW_HPP
#define MSG_WNDW_HPP

#include <irrlicht.h>

using namespace irr;
using namespace gui;
using namespace core;

class MsgWndw
{
public:
    static const int OK_BTN_ID = 590;
    static const int CA_BTN_ID = 591;
private:

    // private variables:
    static MsgWndw*         m_self;

    IGUIWindow*             m_wndw;

    IGUIButton*             m_ok;
    IGUIButton*             m_cancel;
    IGUIStaticText*         m_msg;

    rect<s32>               m_ok_def_r;
    rect<s32>               m_ok_center;

    bool                    exit_on_ok;

    // private functions:
    void    init();

    MsgWndw() {};
public:
    static MsgWndw*  get();
    bool             isVisible()     { return m_wndw->isVisible(); }
                     
    void             showMsg(stringw msg, bool cbtn = false, bool eoo = false);
    void             buttonClicked(bool cancel = false);
};

#endif
