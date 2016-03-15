#include "gui/road_panel.hpp"

#include "editor.hpp"
#include "track.hpp"

#include "viewport/viewport.hpp"
#include "mesh/driveline.hpp"
#include "mesh/road.hpp"
#include "spline/bezier.hpp"
#include "spline/tcr.hpp"

RoadPanel* RoadPanel::m_road_panel = 0;
    
void RoadPanel::setScrollBarsEnabled(bool isEnabled)
{
    if(m_width_sb)
        m_width_sb->setEnabled(isEnabled);
    if(m_detail_sb)
        m_detail_sb->setEnabled(isEnabled); 
    if(m_u_sb)
        m_u_sb->setEnabled(isEnabled);
    if(m_v_sb)
        m_v_sb->setEnabled(isEnabled);
    if(m_u_offset_sb)
        m_u_offset_sb->setEnabled(isEnabled);
    if(m_v_offset_sb)
        m_v_offset_sb->setEnabled(isEnabled);
    
} // setScrollBarsEnabled
// ----------------------------------------------------------------------------
void RoadPanel::init()
{
    m_next_road_mesh_ID = 1;

    Editor* editor = Editor::getEditor();
    path icons     = editor->getIconsLoc();

    IGUIEnvironment* gui_env = editor->getGUIEnv();
    IGUIFont* font = editor->getFont();
    
    m_v_scroll_rp                       = rect<s32>(240, 0, 250, 10);
    m_cb_rp                             = rect<s32>(30, 45, 210, 65);
    m_spline_type_cb_rp                 = rect<s32>(30, 470, 150, 490);
    m_text_field_rp                     = rect<s32>(30, 495, 150, 515);

    m_width_sb_rp                       = rect<s32>(100, 80, 210, 90);
    m_detail_sb_rp                      = rect<s32>(100, 95, 210, 105);
    m_u_sb_rp                           = rect<s32>(100, 128, 210, 138);
    m_v_sb_rp                           = rect<s32>(100, 148, 210, 158);
    m_u_offset_sb_rp                    = rect<s32>(100, 168, 210, 178);
    m_v_offset_sb_rp                    = rect<s32>(100, 188, 210, 198);

    m_closed_road_cb_rp                 = rect<s32>(30, 290, 210, 320);
    m_swap_uv_rp                        = rect<s32>(30,255,210,285);
    
    m_st_road_selected_text_rp          = rect<s32>(10, 10, 200, 30);
    m_st_width_rp                       = rect<s32>(30, 77, 90, 92);
    m_st_detail_rp                      = rect<s32>(30, 92, 90, 117);
    m_bt_texture_rp                     = rect<s32>(30, 123, 90, 143);
    m_bt_add_controlpoint_rp            = rect<s32>(30, 330, 80, 380);
    m_bt_insert_controlpoint_rp         = rect<s32>(95, 330, 145, 380);
    m_bt_finish_point_placing_rp        = rect<s32>(160, 330, 210, 380);
    m_bt_edit_crossroads_section_rp     = rect<s32>(60, 395, 110, 445);
    m_bt_attach_to_driveline_rp         = rect<s32>(130, 395, 180, 445);
    m_bt_create_road_rp                 = rect<s32>(160, 470, 210, 515);
    m_bt_checkline_rp                   = rect<s32>(30, 530, 210, 580);
    
    m_wndw->setRelativePosition(position2di(0, 50));
    
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
    
    m_st_road_selected = gui_env->addStaticText(_(L"Selected Road:"), m_st_road_selected_text_rp,
        false, false, m_wndw, ST_ROAD_SELECTED, false);
    m_st_road_selected->setOverrideFont(font);

    m_cb = gui_env->addComboBox(m_cb_rp, m_wndw, SELECT);
    m_cb->addItem(L"DriveLine", 0);

    m_st_width = gui_env->addStaticText(_(L"Width:"), m_st_width_rp, false, false, m_wndw, WIDTH);
    m_width_sb = gui_env->addScrollBar(true, m_width_sb_rp, m_wndw, WIDTH);
    m_width_sb->setMin(1);
    m_width_sb->setMax(400);
    m_width_sb->setPos(80);

    m_st_detail = gui_env->addStaticText(_(L"Detail:"), m_st_detail_rp, false, false, m_wndw, DETAIL);
    m_detail_sb = gui_env->addScrollBar(true, m_detail_sb_rp, m_wndw, DETAIL);
    m_detail_sb->setMin(1);
    m_detail_sb->setMax(100);
    m_detail_sb->setPos(25);

    m_bt_texture = gui_env->addButton(m_bt_texture_rp, m_wndw, TEXTURE, _(L"Texture"));

    // SCROLLBAR - WRAP U
    m_u_sb = gui_env->addScrollBar(true, m_u_sb_rp, m_wndw, WRAP_U);
    m_u_sb->setMin(1);
    m_u_sb->setMax(15);
    m_u_sb->setPos(1);
    m_u_sb->setLargeStep(2);
    m_u_sb->setSmallStep(1);

    // SCROLLBAR - WRAP V
    m_v_sb = gui_env->addScrollBar(true, m_v_sb_rp, m_wndw, WRAP_V);
    m_v_sb->setMin(1);
    m_v_sb->setMax(100);
    m_v_sb->setPos(25);

    // SCROLLBAR - OFFSET U
    m_u_offset_sb = gui_env->addScrollBar(true, m_u_offset_sb_rp, m_wndw, OFFSET_U);
    m_u_offset_sb->setMin(0);
    m_u_offset_sb->setMax(20);
    m_u_offset_sb->setPos(0);
    m_u_offset_sb->setLargeStep(4);
    m_u_offset_sb->setSmallStep(1);

    // SCROLLBAR - OFFSET V
    m_v_offset_sb = gui_env->addScrollBar(true, m_v_offset_sb_rp, m_wndw, OFFSET_V);
    m_v_offset_sb->setMin(0);
    m_v_offset_sb->setMax(20);
    m_v_offset_sb->setPos(0);
    m_v_offset_sb->setLargeStep(4);
    m_v_offset_sb->setSmallStep(1);

    // CHECKBOX 
    m_swap_uv        = gui_env->addCheckBox(false, m_swap_uv_rp,
                                            m_wndw, SWAP_UV, L"Swap UV");

    m_closed_road_cb = gui_env->addCheckBox(false, m_closed_road_cb_rp,
                                            m_wndw, CLOSED_ROAD, L"Closed Road");

    m_bt_add_controlpoint = gui_env->addButton(m_bt_add_controlpoint_rp, m_wndw, ADD, L"", _(L"Add ControlPoint"));
    m_bt_add_controlpoint->setImage(Editor::loadImg(icons + "dl_add.png"));
    
    m_bt_insert_controlpoint = gui_env->addButton(m_bt_insert_controlpoint_rp, m_wndw, INSERT, L"", _(L"Insert ControlPoint"));
    m_bt_insert_controlpoint->setImage(Editor::loadImg(icons + "dl_insert.png"));
    
    m_bt_finish_point_placing = gui_env->addButton(m_bt_finish_point_placing_rp, m_wndw, EXIT, L"", _(L"Finish Point Placing"));
    m_bt_finish_point_placing->setImage(Editor::loadImg(icons + "dl_ready.png"));

    m_bt_edit_crossroads_section = gui_env->addButton(m_bt_edit_crossroads_section_rp, m_wndw, CROSS_SECTION, L"", _(L"Editor road cross section"));
    m_bt_edit_crossroads_section->setImage(Editor::loadImg(icons + "edit_crossroads_section.png"));
    
    m_bt_attach_to_driveline = gui_env->addButton(m_bt_attach_to_driveline_rp, m_wndw, ATTACH_TO_DL, L"", _(L"Attach to DriveLine"));
    m_bt_attach_to_driveline->setImage(Editor::loadImg(icons + "create-quads.png"));

    m_spline_type_cb = gui_env->addComboBox(m_spline_type_cb_rp, m_wndw);
    m_spline_type_cb->addItem(L"Bezier", 0);
    m_spline_type_cb->addItem(L"Catmull-Rom", 1);

    m_text_field = gui_env->addEditBox(L"RoadMesh_1", m_text_field_rp,true,m_wndw,NAMEBOX);

    m_bt_create_road = gui_env->addButton(m_bt_create_road_rp, m_wndw, CREATE, L"", _(L"Create New Road Mesh"));
    m_bt_create_road->setImage(Editor::loadImg(icons + "road_create.png"));
        
    m_bt_checkline = gui_env->addButton(m_bt_checkline_rp, m_wndw, CHECKLINE, L"CHECKLINE");

    m_insert = false;
    
    m_gui_panel_components.addComponent(m_wndw, m_cb,                           m_cb_rp);
    m_gui_panel_components.addComponent(m_wndw, m_spline_type_cb,               m_spline_type_cb_rp);
    m_gui_panel_components.addComponent(m_wndw, m_text_field,                   m_text_field_rp);
    m_gui_panel_components.addComponent(m_wndw, m_width_sb,                     m_width_sb_rp);
    m_gui_panel_components.addComponent(m_wndw, m_detail_sb,                    m_detail_sb_rp);
    m_gui_panel_components.addComponent(m_wndw, m_u_sb,                         m_u_sb_rp);
    m_gui_panel_components.addComponent(m_wndw, m_v_sb,                         m_v_sb_rp);
    m_gui_panel_components.addComponent(m_wndw, m_u_offset_sb,                  m_u_offset_sb_rp);
    m_gui_panel_components.addComponent(m_wndw, m_v_offset_sb,                  m_v_offset_sb_rp);
    m_gui_panel_components.addComponent(m_wndw, m_closed_road_cb,               m_closed_road_cb_rp);
    m_gui_panel_components.addComponent(m_wndw, m_swap_uv,                      m_swap_uv_rp);
    m_gui_panel_components.addComponent(m_wndw, m_st_road_selected,             m_st_road_selected_text_rp);
    m_gui_panel_components.addComponent(m_wndw, m_st_width,                     m_st_width_rp);
    m_gui_panel_components.addComponent(m_wndw, m_st_detail,                    m_st_detail_rp);
    m_gui_panel_components.addComponent(m_wndw, m_bt_texture,                   m_bt_texture_rp);
    m_gui_panel_components.addComponent(m_wndw, m_bt_add_controlpoint,          m_bt_add_controlpoint_rp);
    m_gui_panel_components.addComponent(m_wndw, m_bt_insert_controlpoint,       m_bt_insert_controlpoint_rp);
    m_gui_panel_components.addComponent(m_wndw, m_bt_finish_point_placing,      m_bt_finish_point_placing_rp);
    m_gui_panel_components.addComponent(m_wndw, m_bt_edit_crossroads_section,   m_bt_edit_crossroads_section_rp);
    m_gui_panel_components.addComponent(m_wndw, m_bt_attach_to_driveline,       m_bt_attach_to_driveline_rp);
    m_gui_panel_components.addComponent(m_wndw, m_bt_create_road,               m_bt_create_road_rp);
    m_gui_panel_components.addComponent(m_wndw, m_bt_checkline,                 m_bt_checkline_rp);

} // init
// ----------------------------------------------------------------------------
void RoadPanel::updatePanelComponentPositions()
{
    int locationAsPixel = (int)(800.0f * (m_v_scroll->getPos() / 800.0f));
    m_gui_panel_components.updateComponentPositions(locationAsPixel);
}
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
    case ADD:
        m_insert = false;
        break;
    case INSERT:
        m_insert = true;
        break;
    case CREATE:
        updateRoadList();
    default:
        break;
    }
} // btnDown

//----------------------------------------------------------------------------
u32 RoadPanel::getSelectedRoadID()
{
    return m_cb->getSelected();
} // getSelectedRoadID

//----------------------------------------------------------------------------
f32 RoadPanel::getWidth()
{
    return m_width_sb->getPos() / 5.0f;
} // getWidth

//----------------------------------------------------------------------------
f32 RoadPanel::getDetail()
{
    return m_detail_sb->getPos() / 100.0f;
} // getWidth

//----------------------------------------------------------------------------
u32 RoadPanel::getTexUCount()
{
    return m_u_sb->getPos();
} // getTexWrapCount

//----------------------------------------------------------------------------
u32 RoadPanel::getTexVCount()
{
    return m_v_sb->getPos();
} // getTexWrapCount

//----------------------------------------------------------------------------
f32 RoadPanel::getTexUOffset()
{
    return (m_u_offset_sb->getPos() / 20.0f);
} // getTexWrapCount

//----------------------------------------------------------------------------
f32 RoadPanel::getTexVOffset()
{
    return (m_v_offset_sb->getPos()/ 20.0f);
} // getTexWrapCount

//-------------------------------------------stepSize---------------------------------
void RoadPanel::updateRoadList()
{
    Track* t = Viewport::get()->getTrack();
    if (t)
    {
        IRoad* r;
        array<IRoad*>* road_list = t->getRoadList();
        m_cb->clear();

        for (u32 i = 0; i < road_list->size(); i++)
        {
            r = (*road_list)[i];
            stringw name = r->getName();
            m_cb->addItem(name.c_str(), i);
        }

        stringw s = stringw(road_list->size());
        m_text_field->setText((stringw("RoadMesh_") + s).c_str());
        m_cb->setSelected(road_list->size()-1);
    }
} // updateRoadList

//----------------------------------------------------------------------------
void RoadPanel::setActiveRoad(u32 ix, IRoad* r)
{
    m_cb->setSelected(ix);
    m_closed_road_cb->setChecked(r->isClosedRoad());
    m_swap_uv->setChecked(r->isSwapOn());
} // setActiveRoad

//----------------------------------------------------------------------------
stringw RoadPanel::getNextRoadType()
{
    return m_spline_type_cb->getItem(m_spline_type_cb->getSelected());
} // getNextRoadType

// ----------------------------------------------------------------------------
void RoadPanel::reallocate(dimension2du ss)
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
