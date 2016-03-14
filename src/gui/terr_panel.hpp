#ifndef TERRPANEL_HPP
#define TERRPANEL_HPP

#include "mesh/mesh_structs.hpp"
#include <irrlicht.h>

#include "gui/simple_gui_objects.hpp"

using namespace irr;
using namespace gui;
using namespace core;

class Sky;

class TerrPanel
{
public:
    // first gui element id
    static const int FGEI = 200;
    
    enum
    {
        RADIUS = FGEI,
        INTENSITY,
        EDGE_1,
        EDGE_2,
        EDGE_3,
        M_T1,
        M_T2,
        M_T3,
        M_T4,
        H_MIN_CHECK_BOX,
        H_MAX_CHECK_BOX,
        H_MIN_EDIT_BOX,
        H_MAX_EDIT_BOX,
        H_BTN,
        T_SOFT_BTN,
        T_HARD_BTN,
        T_BRIGHTNESS_BTN,
        M_TC1,
        M_TC2,
        M_TC3,
        M_TC4,
        S_T1,
        S_T2,
        S_T3,
        S_T4,
        S_T5,
        S_T6,
        V_SCROLL,
        ST_BRUSH,
        ST_RADIUS,
        ST_INTENSITY,
        ST_EDGE,
        ST_SOFT_BRUSH,
        ST_HARD_BRUSH,
        ST_BRIGHTNESS,
        ST_SKYBOX,
        ST_UP,
        ST_DOWN,
        ST_LEFT,
        ST_RIGHT,
        ST_FRONT,
        ST_BACK,
        ST_CHG_TEX_B1,
        ST_CHG_TEX_B2,
        ST_CHG_TEX_B3,
        ST_CHG_TEX_B4,
        ST_SPADE_TEX,
        ST_EDGE_B1,
        ST_EDGE_B2,
        ST_EDGE_B3
    };

private:

    // private variables:

    static TerrPanel*       m_terr_panel;

    IGUIWindow*             m_wndw;

    // panel components
    IGUIScrollBar*          m_h_radius;
    IGUIScrollBar*          m_h_intensity;
    IGUIScrollBar*          m_v_scroll;
    
    IGUIImage*              m_active_edge_frame;

    IGUIEditBox*            m_h_min_value;
    IGUIEditBox*            m_h_max_value;

    IGUICheckBox*           m_h_min_cb;
    IGUICheckBox*           m_h_max_cb;

    IGUIButton*             m_tb1; 
    IGUIButton*             m_tb2; 
    IGUIButton*             m_tb3; 
    IGUIButton*             m_tb4; 

    IGUIButton*             m_sb1;
    IGUIButton*             m_sb2;
    IGUIButton*             m_sb3;
    IGUIButton*             m_sb4;
    IGUIButton*             m_sb5;
    IGUIButton*             m_sb6;
    
    TerrainMod              m_tmod;

    IGUIStaticText*               m_brush_text;
    IGUIStaticText*               m_radius_text;
    IGUIStaticText*               m_intensity_text;
    IGUIStaticText*               m_edge_text;
    IGUIButton*                   m_soft_brush;
    IGUIButton*                   m_hard_brush;
    IGUIButton*                   m_brightness;
    IGUIStaticText*               m_skybox_text;
    IGUIStaticText*               m_up_text;
    IGUIStaticText*               m_down_text;
    IGUIStaticText*               m_left_text;
    IGUIStaticText*               m_right_text;
    IGUIStaticText*               m_front_text;
    IGUIStaticText*               m_back_text;
    IGUIButton*                   m_intensity;
    
    IGUIButton*                   m_change_texture_b1;
    IGUIButton*                   m_change_texture_b2;
    IGUIButton*                   m_change_texture_b3;
    IGUIButton*                   m_change_texture_b4;
    IGUIButton*                   m_spade_texture;
    
    IGUIStaticText*               m_edge_b1;
    IGUIStaticText*               m_edge_b2;
    IGUIStaticText*               m_edge_b3;
    
    // panel component root relative positions
    rect<s32>               m_h_radius_rp                       = rect<s32>(100, 45, 240, 55);
    rect<s32>               m_h_intensity_rp                    = rect<s32>(100, 75, 240, 85);
    
    rect<s32>               m_active_edge_frame_rp              = rect<s32>(200, 95, 240, 135);

    rect<s32>               m_h_min_value_rp                    = rect<s32>(170, 285, 210, 305);
    rect<s32>               m_h_max_value_rp                    = rect<s32>(170, 255, 210, 275);

    rect<s32>               m_h_min_cb_rp                       = rect<s32>(105, 280, 175, 310);
    rect<s32>               m_h_max_cb_rp                       = rect<s32>(105, 250, 175, 280);

    rect<s32>               m_tb1_rp                            = rect<s32>(10,  150, 60,  200); 
    rect<s32>               m_tb2_rp                            = rect<s32>(70,  150, 120, 200); 
    rect<s32>               m_tb3_rp                            = rect<s32>(130, 150, 180, 200); 
    rect<s32>               m_tb4_rp                            = rect<s32>(190, 150, 240, 200); 

    rect<s32>               m_edge_b1_rp                        = rect<s32>(105, 100, 135, 130);
    rect<s32>               m_edge_b2_rp                        = rect<s32>(155, 100, 185, 130);
    rect<s32>               m_edge_b3_rp                        = rect<s32>(205, 100, 235, 130);
    
    rect<s32>               m_sb1_rp                            = rect<s32>(30,  475, 80,  525);
    rect<s32>               m_sb2_rp                            = rect<s32>(95,  475, 145, 525);
    rect<s32>               m_sb3_rp                            = rect<s32>(160, 475, 210, 525);
    rect<s32>               m_sb4_rp                            = rect<s32>(30,  555, 80,  605);
    rect<s32>               m_sb5_rp                            = rect<s32>(95,  555, 145, 605);
    rect<s32>               m_sb6_rp                            = rect<s32>(160, 555, 210, 605);
    
    rect<s32>               m_brush_text_rp                     = rect<s32>(10, 10, 200, 30);
    rect<s32>               m_radius_text_rp                    = rect<s32>(15, 40, 100, 60);
    rect<s32>               m_intensity_text_rp                 = rect<s32>(15, 70, 100, 90);
    rect<s32>               m_edge_text_rp                      = rect<s32>(15, 100, 100, 120);
    rect<s32>               m_soft_brush_text_rp                = rect<s32>(30, 350, 80, 400);
    rect<s32>               m_hard_brush_text_rp                = rect<s32>(95, 350, 145, 400);
    rect<s32>               m_brightness_text_rp                = rect<s32>(160, 350, 210, 400);
    rect<s32>               m_skybox_text_rp                    = rect<s32>(10, 430, 200, 450);
    rect<s32>               m_up_text_rp                        = rect<s32>(30, 455, 80, 475);
    rect<s32>               m_down_text_rp                      = rect<s32>(95, 455, 145, 475);
    rect<s32>               m_left_text_rp                      = rect<s32>(160, 455, 210, 475);
    rect<s32>               m_right_text_rp                     = rect<s32>(30, 535, 80, 555);
    rect<s32>               m_front_text_rp                     = rect<s32>(95, 535, 145, 555);
    rect<s32>               m_back_text_rp                      = rect<s32>(160, 535, 210, 555);
    
    rect<s32>               m_change_texture_b1_rp              = rect<s32>(10, 205, 60,   220);
    rect<s32>               m_change_texture_b2_rp              = rect<s32>(70, 205, 120,  220);
    rect<s32>               m_change_texture_b3_rp              = rect<s32>(130, 205, 180, 220);
    rect<s32>               m_change_texture_b4_rp              = rect<s32>(190, 205, 240, 220);
    rect<s32>               m_spade_texture_rp                  = rect<s32>(30, 255, 80, 305);
 
    // private functions:
    SimpleGuiComponents     m_gui_panel_components;
    TerrPanel() {};

    void init();
    void terrChange(u32 id);
    void skyChange(u32 id);

public:
    static TerrPanel*     getTerrPanel(IGUIWindow* wndw = 0);

    void                  updatePanelComponentPositions();
    TerrainMod*           getTerrainModData()         { return &m_tmod; }
    void                  setScrollBarsEnabled(bool isEnabled);
    void                  btnDown(int btn);
    void                  refreshTerrModData();
    void                  refreshSkyButtons(Sky* sky);
    void                  refreshTerrainTextures(SMaterial m);
    void                  reallocate(dimension2du ss);
};

#endif
