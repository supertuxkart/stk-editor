#include "toolbox/toolbox.hpp"

#include "editor.hpp"

ToolBox* ToolBox::m_toolbox = 0;

// ----------------------------------------------------------------------------
void ToolBox::init()
{
    IGUIEnvironment* env = Editor::getEditor()->getGUIEnv();
    dimension2du ss = Editor::getEditor()->getScreenSize();

    m_boxwnd = env->addWindow(
            rect<s32>(),
            false, L"Stuffs", 0, 0
        );
    m_boxwnd->setMinSize(dimension2du(250, ss.Height-50));
    m_boxwnd->setRelativePosition(position2di(ss.Width-250,50));
    m_boxwnd->setDraggable(false);
    m_boxwnd->getCloseButton()->setVisible(false);
    m_boxwnd->setDrawTitlebar(false);


    // create tab control and tabs
    m_tab = env->addTabControl(
        core::rect<s32>(), m_boxwnd, true, true);

    m_tab->setMinSize(dimension2du(250, ss.Height - 50));
    m_tab->setRelativePosition(position2di(0, 0));

    m_tab->addTab(L"Terrain");
    m_tab->addTab(L"Road");
    m_tab->addTab(L"Env");
    m_tab->addTab(L"Extra");
    m_tab->addTab(L"Blocks");


} // init

// ----------------------------------------------------------------------------
ToolBox* ToolBox::getToolBox()
{
    if (m_toolbox != 0) return m_toolbox;

    m_toolbox = new ToolBox();
    m_toolbox->init();
    return m_toolbox;
} // getToolBox

// ----------------------------------------------------------------------------
void ToolBox::reallocate()
{
    dimension2du ss = Editor::getEditor()->getScreenSize();

    m_boxwnd->setMinSize(dimension2du(250, ss.Height-50));
    m_boxwnd->setRelativePosition(position2di(ss.Width-250,50));
    m_tab->setMinSize(dimension2du(250, ss.Height - 50));

} // reallocate
