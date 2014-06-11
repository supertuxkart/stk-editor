#include "toolbox/terrpanel.hpp"
#include "editor.hpp"

TerrPanel* TerrPanel::m_terr_panel = 0;

// ----------------------------------------------------------------------------
void TerrPanel::init()
{
    IGUIEnvironment* gui_env = Editor::getEditor()->getGUIEnv();
    
    gui_env->addStaticText(L"Height Drawer:", rect<s32>(10, 10, 100, 20), 
                           false, false, m_tab, -1, false);

    gui_env->addStaticText(L"Radius:", rect<s32>(10, 30, 50, 40),
        false, false, m_tab, -1, false);

    gui_env->addStaticText(L"Intensity:", rect<s32>(10, 50, 50, 60),
        false, false, m_tab, -1, false);

    gui_env->addStaticText(L"Edge:", rect<s32>(10, 80, 50, 90),
        false, false, m_tab, -1, false);

    m_h_radius    = gui_env->addScrollBar(true, rect<s32>(60, 30, 240, 40), 
                                         m_tab, H_RADIUS);
    m_h_radius->setMin(1);
    m_h_radius->setMax(100);
    m_h_radius->setSmallStep(1);
    m_h_radius->setLargeStep(10);
    m_h_radius->setPos(60);

    m_h_intensity = gui_env->addScrollBar(true, rect<s32>(60, 50, 240, 60), 
                                         m_tab, H_INTENSITY);
    m_h_intensity->setMin(-100);
    m_h_intensity->setMax(100);
    m_h_intensity->setSmallStep(1);
    m_h_intensity->setLargeStep(10);
    m_h_intensity->setPos(10);

    m_active_edge_frame = gui_env->addImage(rect<s32>(65, 65, 105, 105), m_tab);
    m_active_edge_frame->setImage(Editor::loadImg("img/frame.png"));

    IGUIButton* b1 = gui_env->addButton(rect<s32>(70, 70, 100, 100), m_tab,  H_EDGE_1);
    IGUIButton* b2 = gui_env->addButton(rect<s32>(120, 70, 150, 100), m_tab, H_EDGE_2);
    IGUIButton* b3 = gui_env->addButton(rect<s32>(170, 70, 200, 100), m_tab, H_EDGE_3);
    b1->setImage(Editor::loadImg("img/edge1.png"));
    b2->setImage(Editor::loadImg("img/edge2.png"));
    b3->setImage(Editor::loadImg("img/edge3.png"));

    m_tmod.ID = 0;
    m_tmod.countdown = -1;
    m_tmod.edge_type = 1;
    refreshTerrModData();

} // init

// ----------------------------------------------------------------------------
TerrPanel* TerrPanel::getTerrPanel(IGUITab* tab)
{
    if (m_terr_panel != 0) return m_terr_panel;

    m_terr_panel = new TerrPanel();
    m_terr_panel->m_tab = tab;
    m_terr_panel->init();
    return m_terr_panel;
} // getTerrPanel

// ----------------------------------------------------------------------------
void TerrPanel::btnDown(int btn)
{
    switch (btn)
    {
    case H_EDGE_1:
        m_tmod.edge_type = 1;
        m_active_edge_frame->setRelativePosition(position2di(65, 65));
        break;
    case H_EDGE_2:
        m_tmod.edge_type = 2;
        m_active_edge_frame->setRelativePosition(position2di(115, 65));
        break;
    case H_EDGE_3:
        m_tmod.edge_type = 3;
        m_active_edge_frame->setRelativePosition(position2di(165, 65));
        break;
    }
} // btnDown

// ----------------------------------------------------------------------------
void TerrPanel::refreshTerrModData()
{
    m_tmod.radius = m_h_radius->getPos() / 10.0f;
    m_tmod.dh     = m_h_intensity->getPos() / 10.0f;
}
