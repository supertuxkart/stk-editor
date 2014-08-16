#ifndef ROADPANEL_HPP
#define ROADPANEL_HPP

#include <irrlicht.h>

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
        NAMEBOX
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

    // private functions:

    RoadPanel() {};

    void                  init();

    void                  textureChange();

public:
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
};

#endif
