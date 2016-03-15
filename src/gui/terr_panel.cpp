#include "gui/terr_panel.hpp"
#include "editor.hpp"


#include "viewport/viewport.hpp"
#include "mesh/terrain.hpp"
#include "mesh/sky.hpp"
#include "gui/tex_sel.hpp"
#include "gui/irr_gui_objects.hpp"

TerrPanel* TerrPanel::m_terr_panel = 0;

// TODO: add V_SCROLL?
void TerrPanel::setScrollBarsEnabled(bool isEnabled)
{
    if(m_h_radius)
        m_h_radius->setEnabled(isEnabled);
    if(m_h_intensity)
        m_h_intensity->setEnabled(isEnabled); 
    
} // setScrollBarsEnabled

// ----------------------------------------------------------------------------
void TerrPanel::init()
{
    Editor* editor = Editor::getEditor();

    m_h_radius_rp                       = rect<s32>(100, 45, 240, 55);
    m_h_intensity_rp                    = rect<s32>(100, 75, 240, 85);
    
    m_active_edge_frame_rp              = rect<s32>(200, 95, 240, 135);

    m_h_min_value_rp                    = rect<s32>(170, 285, 210, 305);
    m_h_max_value_rp                    = rect<s32>(170, 255, 210, 275);

    m_h_min_cb_rp                       = rect<s32>(105, 280, 175, 310);
    m_h_max_cb_rp                       = rect<s32>(105, 250, 175, 280);

    m_tb1_rp                            = rect<s32>(10,  150, 60,  200); 
    m_tb2_rp                            = rect<s32>(70,  150, 120, 200); 
    m_tb3_rp                            = rect<s32>(130, 150, 180, 200); 
    m_tb4_rp                            = rect<s32>(190, 150, 240, 200); 

    m_edge_b1_rp                        = rect<s32>(105, 100, 135, 130);
    m_edge_b2_rp                        = rect<s32>(155, 100, 185, 130);
    m_edge_b3_rp                        = rect<s32>(205, 100, 235, 130);
    
    m_sb1_rp                            = rect<s32>(30,  475, 80,  525);
    m_sb2_rp                            = rect<s32>(95,  475, 145, 525);
    m_sb3_rp                            = rect<s32>(160, 475, 210, 525);
    m_sb4_rp                            = rect<s32>(30,  555, 80,  605);
    m_sb5_rp                            = rect<s32>(95,  555, 145, 605);
    m_sb6_rp                            = rect<s32>(160, 555, 210, 605);
    
    m_brush_text_rp                     = rect<s32>(10, 10, 200, 30);
    m_radius_text_rp                    = rect<s32>(15, 40, 100, 60);
    m_intensity_text_rp                 = rect<s32>(15, 70, 100, 90);
    m_edge_text_rp                      = rect<s32>(15, 100, 100, 120);
    m_soft_brush_text_rp                = rect<s32>(30, 350, 80, 400);
    m_hard_brush_text_rp                = rect<s32>(95, 350, 145, 400);
    m_brightness_text_rp                = rect<s32>(160, 350, 210, 400);
    m_skybox_text_rp                    = rect<s32>(10, 430, 200, 450);
    m_up_text_rp                        = rect<s32>(30, 455, 80, 475);
    m_down_text_rp                      = rect<s32>(95, 455, 145, 475);
    m_left_text_rp                      = rect<s32>(160, 455, 210, 475);
    m_right_text_rp                     = rect<s32>(30, 535, 80, 555);
    m_front_text_rp                     = rect<s32>(95, 535, 145, 555);
    m_back_text_rp                      = rect<s32>(160, 535, 210, 555);
    
    m_change_texture_b1_rp              = rect<s32>(10, 205, 60,   220);
    m_change_texture_b2_rp              = rect<s32>(70, 205, 120,  220);
    m_change_texture_b3_rp              = rect<s32>(130, 205, 180, 220);
    m_change_texture_b4_rp              = rect<s32>(190, 205, 240, 220);
    m_spade_texture_rp                  = rect<s32>(30, 255, 80, 305);
    
    IGUIEnvironment* gui_env = editor->getGUIEnv();
    IGUIFont* font           = editor->getFont();
    path icons               = editor->getIconsLoc();
    
    m_wndw->setRelativePosition(position2di(0, 50));
    
    m_brush_text = gui_env->addStaticText(_(L"Brush:"), m_brush_text_rp,
                           false, false, m_wndw, ST_BRUSH, false);
    m_brush_text->setOverrideFont(font);

    m_radius_text = gui_env->addStaticText(_(L"Radius:"), m_radius_text_rp,
                           false, false, m_wndw, ST_RADIUS, false);

    m_h_radius = gui_env->addScrollBar(true, m_h_radius_rp,
        m_wndw, RADIUS);
    m_h_radius->setMin(1);
    m_h_radius->setMax(100);
    m_h_radius->setSmallStep(1);
    m_h_radius->setLargeStep(10);
    m_h_radius->setPos(60);
    
    m_v_scroll = gui_env->addScrollBar(false, rect<s32>(240, 0, 250, 10),
                                       m_wndw, V_SCROLL);
    m_v_scroll->setMin(0);
    m_v_scroll->setMax(800);
    if(editor->getScreenSize().Height < 800) {
        float stepSize = 800.0 * ((editor->getScreenSize().Height - 100) / 800.f);
        m_v_scroll->setMinSize(dimension2du(10, editor->getScreenSize().Height - 100));
        m_v_scroll->setSmallStep(stepSize);
        m_v_scroll->setLargeStep(stepSize);
        m_v_scroll->setVisible(true);
    }
    else
        m_v_scroll->setVisible(false);
        
    
    m_intensity_text = gui_env->addStaticText(_(L"Intensity:"), m_intensity_text_rp,
                           false, false, m_wndw, ST_INTENSITY, false);

    m_h_intensity = gui_env->addScrollBar(true, m_h_intensity_rp,
        m_wndw, INTENSITY);
    m_h_intensity->setMin(1);
    m_h_intensity->setMax(100);
    m_h_intensity->setSmallStep(1);
    m_h_intensity->setLargeStep(10);
    m_h_intensity->setPos(20);

    // edge begin
    IGUIButton* b1 = gui_env->addButton(m_edge_b1_rp, m_wndw, EDGE_1);
    IGUIButton* b2 = gui_env->addButton(m_edge_b2_rp, m_wndw, EDGE_2);
    IGUIButton* b3 = gui_env->addButton(m_edge_b3_rp, m_wndw, EDGE_3);
    b1->setImage(Editor::loadImg(icons + "edge1.png"));
    b2->setImage(Editor::loadImg(icons + "edge2.png"));
    b3->setImage(Editor::loadImg(icons + "edge3.png"));

    m_edge_text = gui_env->addStaticText(_(L"Edge:"), m_edge_text_rp,
        false, false, m_wndw, ST_EDGE, false);

    m_active_edge_frame = gui_env->addImage(m_active_edge_frame_rp, m_wndw);
    m_active_edge_frame->setImage(Editor::loadImg(icons + "frame.png"));
    m_active_edge_frame->setUseAlphaChannel(true);
    // edge end

    m_tb1 = gui_env->addButton(m_tb1_rp, m_wndw, M_T1, L"", _(L"Ground type 1"));
    m_tb2 = gui_env->addButton(m_tb2_rp, m_wndw, M_T2, L"", _(L"Ground type 2"));
    m_tb3 = gui_env->addButton(m_tb3_rp, m_wndw, M_T3, L"", _(L"Ground type 3"));
    m_tb4 = gui_env->addButton(m_tb4_rp, m_wndw, M_T4, L"", _(L"Ground type 4"));

    m_tb1->setImage(Editor::loadImg(L"dirt.jpg"));
    m_tb2->setImage(Editor::loadImg(L"grass2.jpg"));
    m_tb3->setImage(Editor::loadImg(L"rock_brown.jpg"));
    m_tb4->setImage(Editor::loadImg(L"sand2.jpg"));

    m_change_texture_b1 = gui_env->addButton(m_change_texture_b1_rp, m_wndw, M_TC1, L"Vary", _(L"Change Texture"));
    m_change_texture_b2 = gui_env->addButton(m_change_texture_b2_rp, m_wndw, M_TC2, L"Vary", _(L"Change Texture"));
    m_change_texture_b3 = gui_env->addButton(m_change_texture_b3_rp, m_wndw, M_TC3, L"Vary", _(L"Change Texture"));
    m_change_texture_b4 = gui_env->addButton(m_change_texture_b4_rp, m_wndw, M_TC4, L"Vary", _(L"Change Texture"));

    // height modifier begin
    m_spade_texture = gui_env->addButton(m_spade_texture_rp, m_wndw, H_BTN, L"", _(L"Spade"));
    m_spade_texture->setImage(Editor::loadImg(icons + "hm.png"));

    m_h_max_cb = gui_env->addCheckBox(false, m_h_max_cb_rp,
                                      m_wndw, H_MAX_CHECK_BOX, L"Max:");
    m_h_max_value = gui_env->addEditBox(L"", m_h_max_value_rp,
                                        true, m_wndw, H_MAX_EDIT_BOX);

    m_h_max_value->setText(L"3");

    m_h_min_cb = gui_env->addCheckBox(false, m_h_min_cb_rp, 
                                      m_wndw, H_MIN_CHECK_BOX, L"Min:");
    m_h_min_value = gui_env->addEditBox(L"", m_h_min_value_rp,
                                        true, m_wndw, H_MIN_EDIT_BOX);
    m_h_min_value->setText(L"-3");
    // height modifier end

    // BRUSH
    m_soft_brush = gui_env->addButton(m_soft_brush_text_rp, m_wndw,T_SOFT_BTN, L"", _(L"Soft Brush"));
    m_soft_brush->setImage(Editor::loadImg(icons + "sb.png"));

    m_hard_brush = gui_env->addButton(m_hard_brush_text_rp, m_wndw, T_HARD_BTN, L"", _(L"Hard Brush"));
    m_hard_brush->setImage(Editor::loadImg(icons + "hb.png"));

    m_brightness = gui_env->addButton(m_brightness_text_rp, m_wndw, T_BRIGHTNESS_BTN, L"", _(L"Brightness"));
    m_brightness->setImage(Editor::loadImg(icons + "bb.png"));

    // SKYBOX

    m_skybox_text = gui_env->addStaticText(_(L"Skybox:"), m_skybox_text_rp,
        false, false, m_wndw, ST_SKYBOX, false);
    m_skybox_text->setOverrideFont(font);

    m_up_text = gui_env->addStaticText(_(L"Up:"), m_up_text_rp,
        false, false, m_wndw, ST_UP, false);
    m_up_text->setOverrideFont(font);
    
    m_down_text = gui_env->addStaticText(_(L"Down:"), m_down_text_rp,
        false, false, m_wndw, ST_DOWN, false);
    m_down_text->setOverrideFont(font);
    
    m_left_text = gui_env->addStaticText(_(L"Left:"), m_left_text_rp,
        false, false, m_wndw, ST_LEFT, false);
    m_left_text->setOverrideFont(font);
    
    m_right_text = gui_env->addStaticText(_(L"Right:"), m_right_text_rp,
        false, false, m_wndw, ST_RIGHT, false);
    m_right_text->setOverrideFont(font);
    
    m_front_text = gui_env->addStaticText(_(L"Front:"), m_front_text_rp,
        false, false, m_wndw, ST_FRONT, false);
    m_front_text->setOverrideFont(font);
    
    m_back_text = gui_env->addStaticText(_(L"Back:"), m_back_text_rp,
        false, false, m_wndw, ST_BACK, false);
    m_back_text->setOverrideFont(font);

    ITexture* up    = Editor::loadImg("summersky_t.jpg");
    ITexture* down  = Editor::loadImg("summersky_b.jpg");
    ITexture* left  = Editor::loadImg("summersky_w.jpg");
    ITexture* right = Editor::loadImg("summersky_e.jpg");
    ITexture* front = Editor::loadImg("summersky_n.jpg");
    ITexture* back  = Editor::loadImg("summersky_s.jpg");

    m_sb1 = gui_env->addButton(m_sb1_rp, m_wndw, S_T1, L"", _(L"Up"));
    m_sb2 = gui_env->addButton(m_sb2_rp, m_wndw, S_T2, L"", _(L"Down"));
    m_sb3 = gui_env->addButton(m_sb3_rp, m_wndw, S_T3, L"", _(L"Left"));
    m_sb4 = gui_env->addButton(m_sb4_rp, m_wndw, S_T4, L"", _(L"Right"));
    m_sb5 = gui_env->addButton(m_sb5_rp, m_wndw, S_T5, L"", _(L"Front"));
    m_sb6 = gui_env->addButton(m_sb6_rp, m_wndw, S_T6, L"", _(L"Back"));
   
    m_sb1->setImage(up);
    m_sb2->setImage(down);
    m_sb3->setImage(left);
    m_sb4->setImage(right);
    m_sb5->setImage(front);
    m_sb6->setImage(back);

    Sky* sky = new Sky(up, down, left, right, front, back);
    sky->hide();
    Viewport::get()->setSky(sky);

    m_tmod.type      = HEIGHT_MOD;
    m_tmod.countdown = -1;
    m_tmod.edge_type = 3;
    m_tmod.max       = false;
    m_tmod.min       = false;
    m_tmod.col_mask  = SColor(255, 0, 0, 0);
    
    // Attach component to component list for updating
    m_gui_panel_components.addComponent(m_wndw, m_brush_text,        m_brush_text_rp);
    m_gui_panel_components.addComponent(m_wndw, m_radius_text,       m_h_radius_rp);
    m_gui_panel_components.addComponent(m_wndw, m_h_radius,          m_h_radius_rp);
    m_gui_panel_components.addComponent(m_wndw, m_h_intensity,       m_h_intensity_rp);
    m_gui_panel_components.addComponent(m_wndw, m_intensity_text,    m_intensity_text_rp);
    m_gui_panel_components.addComponent(m_wndw, b1,                  m_edge_b1_rp);
    m_gui_panel_components.addComponent(m_wndw, b2,                  m_edge_b2_rp);
    m_gui_panel_components.addComponent(m_wndw, b3,                  m_edge_b3_rp);
    m_gui_panel_components.addComponent(m_wndw, m_edge_text,         m_edge_text_rp);
    m_gui_panel_components.addComponent(m_wndw, m_active_edge_frame, m_active_edge_frame_rp);
    m_gui_panel_components.addComponent(m_wndw, m_tb1,               m_tb1_rp);
    m_gui_panel_components.addComponent(m_wndw, m_tb2,               m_tb2_rp);
    m_gui_panel_components.addComponent(m_wndw, m_tb3,               m_tb3_rp);
    m_gui_panel_components.addComponent(m_wndw, m_tb4,               m_tb4_rp);
    m_gui_panel_components.addComponent(m_wndw, m_change_texture_b1, m_change_texture_b1_rp);
    m_gui_panel_components.addComponent(m_wndw, m_change_texture_b2, m_change_texture_b2_rp);
    m_gui_panel_components.addComponent(m_wndw, m_change_texture_b3, m_change_texture_b3_rp);
    m_gui_panel_components.addComponent(m_wndw, m_change_texture_b4, m_change_texture_b4_rp);
    m_gui_panel_components.addComponent(m_wndw, m_spade_texture,     m_spade_texture_rp);
    m_gui_panel_components.addComponent(m_wndw, m_h_max_cb,          m_h_max_cb_rp);
    m_gui_panel_components.addComponent(m_wndw, m_h_max_value,       m_h_max_value_rp);
    m_gui_panel_components.addComponent(m_wndw, m_h_min_cb,          m_h_min_cb_rp);
    m_gui_panel_components.addComponent(m_wndw, m_h_min_value,       m_h_min_value_rp);
    m_gui_panel_components.addComponent(m_wndw, m_soft_brush,        m_soft_brush_text_rp);
    m_gui_panel_components.addComponent(m_wndw, m_hard_brush,        m_hard_brush_text_rp);
    m_gui_panel_components.addComponent(m_wndw, m_brightness,        m_brightness_text_rp);
    m_gui_panel_components.addComponent(m_wndw, m_skybox_text,       m_skybox_text_rp);
    m_gui_panel_components.addComponent(m_wndw, m_up_text,           m_up_text_rp);
    m_gui_panel_components.addComponent(m_wndw, m_down_text,         m_down_text_rp);
    m_gui_panel_components.addComponent(m_wndw, m_left_text,         m_left_text_rp);
    m_gui_panel_components.addComponent(m_wndw, m_right_text,        m_right_text_rp);
    m_gui_panel_components.addComponent(m_wndw, m_front_text,        m_front_text_rp);
    m_gui_panel_components.addComponent(m_wndw, m_back_text,         m_back_text_rp);
    m_gui_panel_components.addComponent(m_wndw, m_sb1,               m_sb1_rp);
    m_gui_panel_components.addComponent(m_wndw, m_sb2,               m_sb2_rp);
    m_gui_panel_components.addComponent(m_wndw, m_sb3,               m_sb3_rp);
    m_gui_panel_components.addComponent(m_wndw, m_sb4,               m_sb4_rp);
    m_gui_panel_components.addComponent(m_wndw, m_sb5,               m_sb5_rp);
    m_gui_panel_components.addComponent(m_wndw, m_sb6,               m_sb6_rp);
    
    refreshTerrModData();

} // init
// ----------------------------------------------------------------------------
void TerrPanel::updatePanelComponentPositions()
{
    int locationAsPixel = (int)(800.0f * (m_v_scroll->getPos() / 800.0f));
    m_gui_panel_components.updateComponentPositions(locationAsPixel);
}
// ----------------------------------------------------------------------------
void TerrPanel::terrChange(u32 id)
{
    Terrain* terr = Viewport::get()->getTerrain();
    
    TexSel* ts = TexSel::getTexSel();
    switch (id)
    {
    case M_TC1:
        ts->subscribe(m_tb1);
        terr->setSubIx(2);
        break;
    case M_TC2:
        ts->subscribe(m_tb2);
        terr->setSubIx(3);
        break;
    case M_TC3:
        ts->subscribe(m_tb3);
        terr->setSubIx(4);
        break;
    case M_TC4:
        ts->subscribe(m_tb4);
        terr->setSubIx(5);
        break;
    }
    ts->subscribe(terr);
} // terrChange

// ----------------------------------------------------------------------------
void TerrPanel::skyChange(u32 id)
{
    Sky* sky = Viewport::get()->getSky();

    TexSel* ts = TexSel::getTexSel();
    switch (id)
    {
    case S_T1:
        ts->subscribe(m_sb1);
        sky->setSubIx(1);
        break;
    case S_T2:
        ts->subscribe(m_sb2);
        sky->setSubIx(2);
        break;
    case S_T3:
        ts->subscribe(m_sb3);
        sky->setSubIx(3);
        break;
    case S_T4:
        ts->subscribe(m_sb4);
        sky->setSubIx(4);
        break;
    case S_T5:
        ts->subscribe(m_sb5);
        sky->setSubIx(5);
        break;
    case S_T6:
        ts->subscribe(m_sb6);
        sky->setSubIx(6);
        break;
    }
    ts->subscribe(sky);
} // skyChange

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
        m_tmod.col_mask = SColor(0, 255, 0, 0);
        break;
    case M_T2:
        m_tmod.col_mask = SColor(0, 0, 255, 0);
        break;
    case M_T3:
        m_tmod.col_mask = SColor(0, 0, 0, 255);
        break;
    case M_T4:
        m_tmod.col_mask = SColor(255, 0, 0, 0);
        break;
    case M_TC1:
    case M_TC2:
    case M_TC3:
    case M_TC4:
        terrChange(btn);
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
    case S_T1:
    case S_T2:
    case S_T3:
    case S_T4:
    case S_T5:
    case S_T6:
        skyChange(btn);
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
} // refreshTerrModData

// ----------------------------------------------------------------------------
void TerrPanel::refreshSkyButtons(Sky* sky)
{
    m_sb1->setImage(sky->getTex(1));
    m_sb2->setImage(sky->getTex(2));
    m_sb3->setImage(sky->getTex(3));
    m_sb4->setImage(sky->getTex(4));
    m_sb5->setImage(sky->getTex(5));
    m_sb6->setImage(sky->getTex(6));
} // refreshSkyButtons

// ----------------------------------------------------------------------------
void TerrPanel::refreshTerrainTextures(SMaterial m)
{
    m_tb1->setImage(m.getTexture(2));
    m_tb2->setImage(m.getTexture(3));
    m_tb3->setImage(m.getTexture(4));
    m_tb4->setImage(m.getTexture(5));
} // refreshTerrainTextures

// ----------------------------------------------------------------------------
void TerrPanel::reallocate(dimension2du ss)
{
    if(ss.Height < 800) {
        m_v_scroll->setMinSize(dimension2du(10, ss.Height - 100));
        m_v_scroll->setMin(0);
        m_v_scroll->setMax(800);
        float stepSize = 800.0 * ((ss.Height - 100) / 800.f);
        m_v_scroll->setSmallStep(stepSize);
        m_v_scroll->setLargeStep(stepSize);
        m_v_scroll->setVisible(true);
        updatePanelComponentPositions();
    }
    else {
        m_v_scroll->setPos(0.0);
        updatePanelComponentPositions();
        m_v_scroll->setVisible(false);
    }
    
    m_wndw->setMinSize(dimension2du(250, ss.Height - 50));
} // reallocate
