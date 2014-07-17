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
        DL_ADD = FGEI,
        DL_INSERT,
        DL_EXIT,
        DL_SELECT,
        DL_CREATE,
        DL_WIDTH,
        DL_DETAIL,
        DL_TEX_CHANGE
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
    void                  updateRoadList();

    stringw               getNextRoadType();
    stringw               getNextRoadName() { return m_text_field->getText();         }

};

#endif
