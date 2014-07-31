#include "gui/msg_wndw.hpp"

#include "editor.hpp"

MsgWndw* MsgWndw::m_self = 0;

// ----------------------------------------------------------------------------
void MsgWndw::init()
{
    IGUIEnvironment* gui_env = Editor::getEditor()->getGUIEnv();
    dimension2du ss = Editor::getEditor()->getScreenSize();

    rect<s32> frame = rect<s32>((s32)(ss.Width / 2.0f - 130),
        (s32)(ss.Height / 2.0f - 75),
        (s32)(ss.Width / 2.0f + 130),
        (s32)(ss.Height / 2.0f + 75));
    m_wndw = gui_env->addWindow(frame, false, _(L""), 0);
    rect<s32> cancel_def_r;

    m_ok_def_r     = rect<s32>(48, 110, 102, 130);
    cancel_def_r   = rect<s32>(148, 110, 202, 130);
    m_ok_center    = rect<s32>(105, 110,155,130);
    
    m_ok     = gui_env->addButton(m_ok_def_r, m_wndw, OK_BTN_ID, _(L"Ok"));
    m_cancel = gui_env->addButton(cancel_def_r, m_wndw, CA_BTN_ID, _(L"Cancel"));

    m_msg = gui_env->addStaticText(L"", rect<s32>(10, 20, 250, 100), false, true, m_wndw);
    m_msg->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);

    m_wndw->setDrawTitlebar(false);
    m_wndw->getCloseButton()->setVisible(false);

    m_wndw->setVisible(false);
} // init

// ----------------------------------------------------------------------------
MsgWndw* MsgWndw::get()
{
    if (m_self != 0) return m_self;

    m_self = new MsgWndw();
    m_self->init();
    return m_self;
} // getToolBox

// ----------------------------------------------------------------------------
void MsgWndw::showMsg(stringw msg, bool cbtn, bool eoo)
{
    m_wndw->setVisible(true);
    exit_on_ok = eoo;
    m_msg->setText(msg.c_str());
    if (cbtn)
    {
        m_cancel->setVisible(true);
        m_ok->setRelativePosition(m_ok_def_r.UpperLeftCorner);
    }
    else
    {
        m_cancel->setVisible(false);
        m_ok->setRelativePosition(m_ok_center.UpperLeftCorner);
    }
} // showMsg

// ----------------------------------------------------------------------------
void MsgWndw::buttonClicked(bool cancel)
{
    if (cancel || !exit_on_ok)
        m_wndw->setVisible(false);
    else Editor::getEditor()->getDevice()->closeDevice();
} // buttonClicked
