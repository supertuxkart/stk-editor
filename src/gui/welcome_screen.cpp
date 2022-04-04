#include "gui/welcome_screen.hpp"

#include "editor.hpp"


WelcomeScreen* WelcomeScreen::m_self = 0;

// ----------------------------------------------------------------------------
void WelcomeScreen::init()
{
    IGUIEnvironment* gui_env = Editor::getEditor()->getGUIEnv();
    dimension2du ss = Editor::getEditor()->getScreenSize();

    rect<s32> frame = rect<s32>((s32)(ss.Width / 2.0f - 300),
        (s32)(ss.Height / 2.0f - 200),
        (s32)(ss.Width / 2.0f + 300),
        (s32)(ss.Height / 2.0f + 200));
    m_wndw = gui_env->addWindow(frame, false, _(L"New Track"), 0);

    gui_env->addStaticText(_(L"Welcome!"), rect<s32>(260, 20, 380, 40),
                                                   false, true, m_wndw);

    m_lb = gui_env->addListBox(rect<s32>(20, 50, 580, 300), m_wndw);

    IGUIButton* bos = gui_env->addButton(rect<s32>(200, 310, 400, 350),
                                         m_wndw, FBTN_ID, _(L"Open Selected"));

    gui_env->addButton(rect<s32>(302.5f, 355, 400, 390),
                    m_wndw, FBTN_ID + 1, _(L"Open"));

    gui_env->addButton(rect<s32>(200, 355, 297.5f, 390),
                     m_wndw, FBTN_ID + 2, _(L"New"));


    std::list<stringc> list;
    std::list<stringc>::iterator it;
    list = Editor::getEditor()->readRecentlyOpenedList();
    for (it = list.begin(); it != list.end(); it++)
    {
        stringw s = *it;
        m_lb->addItem(s.c_str());
    }
    if (list.empty())
        bos->setEnabled(false);

    m_wndw->setDrawTitlebar(false);
    m_wndw->getCloseButton()->setVisible(false);

} // init

// ----------------------------------------------------------------------------
WelcomeScreen* WelcomeScreen::get()
{
    if (m_self != 0) return m_self;

    m_self = new WelcomeScreen();
    m_self->init();
    return m_self;
} // getToolBox

// ----------------------------------------------------------------------------
stringc WelcomeScreen::getActiveText()
{
    return m_lb->getListItem(m_lb->getSelected());
} // getActiveText
