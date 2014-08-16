#include "gui/road_panel.hpp"

#include "editor.hpp"
#include "track.hpp"

#include "viewport/viewport.hpp"
#include "mesh/driveline.hpp"
#include "mesh/road.hpp"
#include "spline/bezier.hpp"
#include "spline/tcr.hpp"

RoadPanel* RoadPanel::m_road_panel = 0;

// ----------------------------------------------------------------------------
void RoadPanel::init()
{
    m_next_road_mesh_ID = 1;

    IGUIEnvironment* gui_env = Editor::getEditor()->getGUIEnv();
    IGUIFont* font = gui_env->getFont(L"font/font.xml");
    m_wndw->setRelativePosition(position2di(0, 50));
    gui_env->addStaticText(_(L"Selected Road:"), rect<s32>(10, 10, 200, 30),
        false, false, m_wndw, -1, false)->setOverrideFont(font);

    m_cb = gui_env->addComboBox(rect<s32>(30, 45, 210, 65), m_wndw, SELECT);
    m_cb->addItem(L"DriveLine", 0);

    gui_env->addStaticText(_(L"Width:"), rect<s32>(30, 77, 90, 92), false, false, m_wndw);
    m_width_sb = gui_env->addScrollBar(true, rect<s32>(100, 80, 210, 90), m_wndw, WIDTH);
    m_width_sb->setMin(1);
    m_width_sb->setMax(400);
    m_width_sb->setPos(80);

    gui_env->addStaticText(_(L"Detail:"), rect<s32>(30, 92, 90, 117), false, false, m_wndw);
    m_detail_sb = gui_env->addScrollBar(true, rect<s32>(100, 95, 210, 105), m_wndw, DETAIL);
    m_detail_sb->setMin(1);
    m_detail_sb->setMax(100);
    m_detail_sb->setPos(25);

    gui_env->addButton(rect<s32>(30, 123, 90, 143), m_wndw, TEXTURE, _(L"Texture"));

    // SCROLLBAR - WRAP U
    m_u_sb = gui_env->addScrollBar(true, rect<s32>(100, 128, 210, 138), m_wndw, WRAP_U);
    m_u_sb->setMin(1);
    m_u_sb->setMax(15);
    m_u_sb->setPos(1);
    m_u_sb->setLargeStep(2);
    m_u_sb->setSmallStep(1);

    // SCROLLBAR - WRAP V
    m_v_sb = gui_env->addScrollBar(true, rect<s32>(100, 148, 210, 158), m_wndw, WRAP_V);
    m_v_sb->setMin(1);
    m_v_sb->setMax(100);
    m_v_sb->setPos(25);

    // SCROLLBAR - OFFSET U
    m_u_offset_sb = gui_env->addScrollBar(true, rect<s32>(100, 168, 210, 178), m_wndw, OFFSET_U);
    m_u_offset_sb->setMin(0);
    m_u_offset_sb->setMax(20);
    m_u_offset_sb->setPos(0);
    m_u_offset_sb->setLargeStep(4);
    m_u_offset_sb->setSmallStep(1);

    // SCROLLBAR - OFFSET V
    m_v_offset_sb = gui_env->addScrollBar(true, rect<s32>(100, 188, 210, 198), m_wndw, OFFSET_V);
    m_v_offset_sb->setMin(0);
    m_v_offset_sb->setMax(20);
    m_v_offset_sb->setPos(0);
    m_v_offset_sb->setLargeStep(4);
    m_v_offset_sb->setSmallStep(1);

    // CHECKBOX 
    m_swap_uv        = gui_env->addCheckBox(false, rect<s32>(30,255,210,285),
                                            m_wndw, SWAP_UV, L"Swap UV");

    m_closed_road_cb = gui_env->addCheckBox(false, rect<s32>(30, 290, 210, 320),
                                            m_wndw, CLOSED_ROAD, L"Closed Road");

    gui_env->addButton(rect<s32>(30, 330, 80, 380), m_wndw, ADD, L"", _(L"Add ControlPoint"))
        ->setImage(Editor::loadImg("img/dl_add.png"));
    gui_env->addButton(rect<s32>(95, 330, 145, 380), m_wndw, INSERT, L"", _(L"Insert ControlPoint"))
        ->setImage(Editor::loadImg("img/dl_insert.png"));
    gui_env->addButton(rect<s32>(160, 330, 210, 380), m_wndw, EXIT, L"", _(L"Finish Point Placing"))
        ->setImage(Editor::loadImg("img/dl_ready.png"));

    gui_env->addButton(rect<s32>(60, 395, 110, 445), m_wndw, CROSS_SECTION, L"", _(L"Editor road cross section"))
        ->setImage(Editor::loadImg("img/edit_crossroads_section.png"));
    gui_env->addButton(rect<s32>(130, 395, 180, 445), m_wndw, ATTACH_TO_DL, L"", _(L"Attach to DriveLine"))
        ->setImage(Editor::loadImg("img/create-quads.png"));

    m_spline_type_cb = gui_env->addComboBox(rect<s32>(30, 470, 150, 490), m_wndw);
    m_spline_type_cb->addItem(L"Bezier", 0);
    m_spline_type_cb->addItem(L"Catmull-Rom", 1);

    m_text_field = gui_env->addEditBox(L"RoadMesh_1",rect<s32>(30, 495, 150, 515),true,m_wndw,NAMEBOX);

    gui_env->addButton(rect<s32>(160, 470, 210, 515), m_wndw, CREATE, L"", _(L"Create New Road Mesh"))
        ->setImage(Editor::loadImg("img/road_create.png"));
        
    gui_env->addButton(rect<s32>(30, 530, 210, 580), m_wndw, CHECKLINE, L"CHECKLINE");

    m_insert = false;

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

//----------------------------------------------------------------------------
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
    m_wndw->setMinSize(dimension2du(250, ss.Height - 50));
} // reallocate
