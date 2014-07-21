#include "gui/welcome_screen.hpp"

#include "editor.hpp"


WelcomeScreen* WelcomeScreen::m_self = 0;

// ----------------------------------------------------------------------------
void WelcomeScreen::init()
{
    IGUIEnvironment* gui_env = Editor::getEditor()->getGUIEnv();
    IGUIFont* font = gui_env->getFont(L"font/font.xml");
    dimension2du ss = Editor::getEditor()->getScreenSize();

    rect<s32> frame = rect<s32>((s32)(ss.Width / 2.0f - 150),
        (s32)(ss.Height / 2.0f - 100),
        (s32)(ss.Width / 2.0f + 150),
        (s32)(ss.Height / 2.0f + 175));
    m_wndw = gui_env->addWindow(frame, false, L"New Track", 0);

    gui_env->addStaticText(L"Welcome!", rect<s32>(110, 20, 190, 40), false, true, m_wndw);

    m_lb = gui_env->addListBox(rect<s32>(20, 50, 280, 180), m_wndw);

    gui_env->addButton(rect<s32>(70, 200, 230, 220), m_wndw, FBTN_ID, L"Open Selected");
    
    gui_env->addButton(rect<s32>(70,  240, 125, 265), m_wndw, FBTN_ID + 1, L"Open");
    gui_env->addButton(rect<s32>(175, 240, 230, 265), m_wndw, FBTN_ID + 2, L"New");


    std::list<stringc> list;
    std::list<stringc>::iterator it;
    list = Editor::getEditor()->readRecentlyOpenedList();

    for (it = list.begin(); it != list.end(); it++)
    {
        stringw s = *it;
        m_lb->addItem(s.c_str());
    }    

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
