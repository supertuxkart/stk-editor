#include "toolbox/roadpanel.hpp"
#include "editor.hpp"

RoadPanel* RoadPanel::m_road_panel = 0;

// ----------------------------------------------------------------------------
void RoadPanel::init()
{
    IGUIEnvironment* gui_env = Editor::getEditor()->getGUIEnv();

    IGUIFont* font = gui_env->getFont(L"font/font1.png");

    m_wndw->setRelativePosition(position2di(0, 50));

    gui_env->addStaticText(L"DriveLine:", rect<s32>(10, 10, 200, 30),
        false, false, m_wndw, -1, false)->setOverrideFont(font);

    gui_env->addButton(rect<s32>(30, 50, 80, 100), m_wndw, DL_ADD)
        ->setImage(Editor::loadImg("img/dl_add.png"));

    gui_env->addButton(rect<s32>(95, 50, 145, 100), m_wndw, DL_INSERT)
        ->setImage(Editor::loadImg("img/dl_insert.png"));

    gui_env->addButton(rect<s32>(160, 50, 210, 100), m_wndw, DL_EXIT)
        ->setImage(Editor::loadImg("img/dl_ready.png"));
    

} // init

// ----------------------------------------------------------------------------
RoadPanel* RoadPanel::getRoadPanel(IGUIWindow* wndw)
{
    if (m_road_panel != 0) return m_road_panel;

    m_road_panel = new RoadPanel();
    m_road_panel->m_wndw = wndw;
    m_road_panel->init();
    return m_road_panel;
} // getTerrPanel

// ----------------------------------------------------------------------------
void RoadPanel::btnDown(int btn)
{
    switch (btn)
    {
    DL_ADD:
        m_insert = false;
        break;
    DL_INSERT:
        m_insert = true;
        break;
    default:
        break;
    }
} // btnDown
