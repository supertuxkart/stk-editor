#include "gui/welcome_screen.hpp"

#include "editor.hpp"


WelcomeScreen* WelcomeScreen::m_self = 0;

// ----------------------------------------------------------------------------
void WelcomeScreen::init()
{
    IGUIEnvironment* gui_env = Editor::getEditor()->getGUIEnv();
    IGUIFont* font = gui_env->getFont(L"font/font1.png");
    dimension2du ss = Editor::getEditor()->getScreenSize();

    rect<s32> frame = rect<s32>((s32)(ss.Width  / 2.0f - 150), 
                                (s32)(ss.Height / 2.0f - 100),
                                (s32)(ss.Width  / 2.0f + 150),
                                (s32)(ss.Height / 2.0f + 100));
    m_wndw = gui_env->addWindow(frame,false, L"New Track", 0);

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

