#ifndef ROADPANEL_HPP
#define ROADPANEL_HPP

#include <irrlicht.h>

#include "gui/simple_gui_objects.hpp"

using namespace irr;
using namespace gui;
using namespace core;

class IRoad;
class DriveLine;
class Track;

class RoadPanel
{
public:
    // first gui element id
    static const int FGEI = 400;

    enum
    {
        ADD = FGEI,
        INSERT,
        EXIT,
        CROSS_SECTION,
        ATTACH_TO_DL,
        SELECT,
        CREATE,
        WIDTH,
        DETAIL,
        TEXTURE,
        WRAP_U,
        WRAP_V,
        OFFSET_U,
        OFFSET_V,
        CHECKLINE,
        CLOSED_ROAD,
        SWAP_UV,
        NAMEBOX,
        ST_ROAD_SELECTED,
        //CB_WIDTH, WIDTH
        //CB_DETAIL, DETAIL
        //BT_TEXTURE, TEXTURE
        //BT_ADD_CONTROLPOINT, ADD
        //BT_INSERT_CONTROLPOINT, INSERT
        //BT_FINISH_POINT_PLACING, EXIT
        //BT_EDIT_CROSSROADS_SECTION, CROSS_SECTION
        //BT_ATTACH_TO_DRIVELINE, ATTACH_TO_DL
        //BT_CREATE_ROAD, CREATE
        //BT_CHECKLINE
        V_SCROLL
    };

private:

    // private variables:

    static RoadPanel*            m_road_panel;

    Track*                       m_track;
                                 
    IGUIWindow*                  m_wndw;
                                 
    IGUIComboBox*                m_cb;
    IGUIComboBox*                m_spline_type_cb;
    IGUIEditBox*                 m_text_field;

    IGUIScrollBar*               m_width_sb;
    IGUIScrollBar*               m_detail_sb;
    IGUIScrollBar*               m_u_sb;
    IGUIScrollBar*               m_v_sb;
    IGUIScrollBar*               m_u_offset_sb;
    IGUIScrollBar*               m_v_offset_sb;

    IGUICheckBox*                m_closed_road_cb;
    IGUICheckBox*                m_swap_uv;
                                 
    unsigned int                 m_next_road_mesh_ID;
    bool                         m_insert;
    
    IGUIStaticText*             m_st_road_selected;
    IGUIStaticText*             m_st_width;
    IGUIStaticText*             m_st_detail;
    IGUIButton*                 m_bt_texture;
    IGUIButton*                 m_bt_add_controlpoint;
    IGUIButton*                 m_bt_insert_controlpoint;
    IGUIButton*                 m_bt_finish_point_placing;
    IGUIButton*                 m_bt_edit_crossroads_section;
    IGUIButton*                 m_bt_attach_to_driveline;
    IGUIButton*                 m_bt_create_road;
    IGUIButton*                 m_bt_checkline;

    IGUIScrollBar*              m_v_scroll;
    
    // root relative positions of components
    rect<s32>                   m_v_scroll_rp;
    rect<s32>                   m_cb_rp;
    rect<s32>                   m_spline_type_cb_rp;
    rect<s32>                   m_text_field_rp;

    rect<s32>                   m_width_sb_rp;
    rect<s32>                   m_detail_sb_rp;
    rect<s32>                   m_u_sb_rp;
    rect<s32>                   m_v_sb_rp;
    rect<s32>                   m_u_offset_sb_rp;
    rect<s32>                   m_v_offset_sb_rp;

    rect<s32>                   m_closed_road_cb_rp;
    rect<s32>                   m_swap_uv_rp;
    
    rect<s32>                   m_st_road_selected_text_rp;
    rect<s32>                   m_st_width_rp;
    rect<s32>                   m_st_detail_rp;
    rect<s32>                   m_bt_texture_rp;
    rect<s32>                   m_bt_add_controlpoint_rp;
    rect<s32>                   m_bt_insert_controlpoint_rp;
    rect<s32>                   m_bt_finish_point_placing_rp;
    rect<s32>                   m_bt_edit_crossroads_section_rp;
    rect<s32>                   m_bt_attach_to_driveline_rp;
    rect<s32>                   m_bt_create_road_rp;
    rect<s32>                   m_bt_checkline_rp;
    // private functions:

    SimpleGuiComponents         m_gui_panel_components;
    
    RoadPanel() {};

    void                  init();

    void                  textureChange();

public:
    void                  setScrollBarsEnabled(bool isEnabled);
    static RoadPanel*     getRoadPanel(IGUIWindow* wndw = 0);
    void                  btnDown(int btn);
    bool                  isInsertMode()    { return m_insert; }
    u32                   getSelectedRoadID();
    f32                   getWidth();
    f32                   getDetail();
    u32                   getTexUCount();
    u32                   getTexVCount();
    f32                   getTexUOffset();
    f32                   getTexVOffset();
    void                  updateRoadList();
    void                  reallocate(dimension2du ss);

    void                  setActiveRoad(u32 ix, IRoad* r);

    stringw               getNextRoadType();
    stringw               getNextRoadName() { return m_text_field->getText();       }
    bool                  getClosedState()  { return m_closed_road_cb->isChecked(); }
    bool                  getSwapState()    { return m_swap_uv->isChecked();        }
    void                  updatePanelComponentPositions();
};

#endif
