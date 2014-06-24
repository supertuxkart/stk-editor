#include "toolbox/terrpanel.hpp"
#include "editor.hpp"

TerrPanel* TerrPanel::m_terr_panel = 0;

// ----------------------------------------------------------------------------
void TerrPanel::init()
{
    IGUIEnvironment* gui_env = Editor::getEditor()->getGUIEnv();

    IGUIFont* font = gui_env->getFont(L"font/font1.png");

    m_wndw->setRelativePosition(position2di(0, 50));
    
    gui_env->addStaticText(L"Brush:", rect<s32>(10, 10, 200, 30), 
                           false, false, m_wndw, -1, false)->setOverrideFont(font);

    gui_env->addStaticText(L"Radius:", rect<s32>(15, 40, 100, 60),
                           false, false, m_wndw, -1, false);

    m_h_radius = gui_env->addScrollBar(true, rect<s32>(100, 45, 240, 55),
        m_wndw, RADIUS);
    m_h_radius->setMin(1);
    m_h_radius->setMax(100);
    m_h_radius->setSmallStep(1);
    m_h_radius->setLargeStep(10);
    m_h_radius->setPos(60);

    gui_env->addStaticText(L"Intensity:", rect<s32>(15, 70, 100, 90),
                           false, false, m_wndw, -1, false);

    m_h_intensity = gui_env->addScrollBar(true, rect<s32>(100, 75, 240, 85),
        m_wndw, INTENSITY);
    m_h_intensity->setMin(1);
    m_h_intensity->setMax(100);
    m_h_intensity->setSmallStep(1);
    m_h_intensity->setLargeStep(10);
    m_h_intensity->setPos(20);

    // edge begin
    IGUIButton* b1 = gui_env->addButton(rect<s32>(105, 100, 135, 130), m_wndw, EDGE_1);
    IGUIButton* b2 = gui_env->addButton(rect<s32>(155, 100, 185, 130), m_wndw, EDGE_2);
    IGUIButton* b3 = gui_env->addButton(rect<s32>(205, 100, 235, 130), m_wndw, EDGE_3);
    b1->setImage(Editor::loadImg("img/edge1.png"));
    b2->setImage(Editor::loadImg("img/edge2.png"));
    b3->setImage(Editor::loadImg("img/edge3.png"));

    gui_env->addStaticText(L"Edge:", rect<s32>(15, 100, 100, 120),
        false, false, m_wndw, -1, false);

    m_active_edge_frame = gui_env->addImage(rect<s32>(200, 95, 240, 135), m_wndw);
    m_active_edge_frame->setImage(Editor::loadImg("img/frame.png"));
    m_active_edge_frame->setUseAlphaChannel(true);
    // edge end

    IGUIButton* b4 = gui_env->addButton(rect<s32>(10,  150, 60,  200), m_wndw, M_T1);
    IGUIButton* b5 = gui_env->addButton(rect<s32>(70,  150, 120, 200), m_wndw, M_T2);
    IGUIButton* b6 = gui_env->addButton(rect<s32>(130, 150, 180, 200), m_wndw, M_T3);
    IGUIButton* b7 = gui_env->addButton(rect<s32>(190, 150, 240, 200), m_wndw, M_T4);

    b4->setImage(Editor::loadImg(L"libraries/terrain/t1.png"));
    b5->setImage(Editor::loadImg(L"libraries/terrain/t2.jpg"));
    b6->setImage(Editor::loadImg(L"libraries/terrain/t3.jpg"));
    b7->setImage(Editor::loadImg(L"libraries/terrain/t4.jpg"));

    // height modifier begin
    gui_env->addButton(rect<s32>(30, 255, 80, 305), m_wndw,H_BTN)
        ->setImage(Editor::loadImg("img/hm.png"));

    m_h_max_cb = gui_env->addCheckBox(false, rect<s32>(105, 250, 175, 280),
                                      m_wndw, H_MAX_CHECK_BOX, L"Max:");
    m_h_max_value = gui_env->addEditBox(L"", rect<s32>(170, 255, 210, 275),
                                        true, m_wndw, H_MAX_EDIT_BOX);

    m_h_max_value->setText(L"3");

    m_h_min_cb = gui_env->addCheckBox(false, rect<s32>(105, 280, 175, 310), 
                                      m_wndw, H_MIN_CHECK_BOX, L"Min:");
    m_h_min_value = gui_env->addEditBox(L"", rect<s32>(170, 285, 210, 305),
                                        true, m_wndw, H_MIN_EDIT_BOX);
    m_h_min_value->setText(L"-3");
    // height modifier end

    gui_env->addButton(rect<s32>(30, 350, 80, 400), m_wndw,T_SOFT_BTN)
        ->setImage(Editor::loadImg("img/sb.png"));

    gui_env->addButton(rect<s32>(95, 350, 145, 400), m_wndw, T_HARD_BTN)
        ->setImage(Editor::loadImg("img/hb.png"));

    gui_env->addButton(rect<s32>(160, 350, 210, 400), m_wndw, T_BRIGHTNESS_BTN)
        ->setImage(Editor::loadImg("img/bb.png"));

    m_tmod.type      = HEIGHT_MOD;
    m_tmod.countdown = -1;
    m_tmod.edge_type = 3;
    m_tmod.max       = false;
    m_tmod.min       = false;
    m_tmod.col_mask  = SColor(255, 0, 0, 0);
    refreshTerrModData();

} // init

// ----------------------------------------------------------------------------
TerrPanel* TerrPanel::getTerrPanel(IGUIWindow* wndw)
{
    if (m_terr_panel != 0) return m_terr_panel;

    m_terr_panel = new TerrPanel();
    m_terr_panel->m_wndw = wndw;
    m_terr_panel->init();
    return m_terr_panel;
} // getTerrPanel

// ----------------------------------------------------------------------------
void TerrPanel::btnDown(int btn)
{
    switch (btn)
    {
    case EDGE_1:
        m_tmod.edge_type = 1;
        m_active_edge_frame->setRelativePosition(position2di(100, 95));
        break;
    case EDGE_2:
        m_tmod.edge_type = 2;
        m_active_edge_frame->setRelativePosition(position2di(150, 95));
        break;
    case EDGE_3:
        m_tmod.edge_type = 3;
        m_active_edge_frame->setRelativePosition(position2di(200, 95));
        break;
    case M_T1:
        m_tmod.col_mask = SColor(255, 0, 0, 0);
        break;
    case M_T2:
        m_tmod.col_mask = SColor(0, 255, 0, 0);
        break;
    case M_T3:
        m_tmod.col_mask = SColor(0, 0, 255, 0);
        break;
    case M_T4:
        m_tmod.col_mask = SColor(0, 0, 0, 255);
        break;
    case TerrPanel::H_BTN:
        m_tmod.type = HEIGHT_MOD;
        break;
    case TerrPanel::T_SOFT_BTN:
        m_tmod.type = SOFT_BRUSH;
        break;
    case TerrPanel::T_HARD_BTN:
        m_tmod.type = HARD_BRUSH;
        break;
    case TerrPanel::T_BRIGHTNESS_BTN:
        m_tmod.type = BRIGHTNESS_MOD;
        break;
    default:
        break;
    }
} // btnDown

// ----------------------------------------------------------------------------
void TerrPanel::refreshTerrModData()
{
    m_tmod.radius = m_h_radius->getPos() / 10.0f;
    m_tmod.dh     = m_h_intensity->getPos() / 200.0f;
    m_tmod.max    = m_h_max_cb->isChecked();
    m_tmod.min    = m_h_min_cb->isChecked();
    
    m_tmod.max_v = (float) atof(((stringc)m_h_max_value->getText()).c_str());
    m_tmod.min_v = (float) atof(((stringc)m_h_min_value->getText()).c_str());
}
