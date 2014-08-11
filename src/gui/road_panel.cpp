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

    gui_env->addButton(rect<s32>(30, 123, 90, 143), m_wndw, TEX_CHANGE, _(L"Texture"));

    m_texture_sb = gui_env->addScrollBar(true, rect<s32>(100, 128, 210, 138), m_wndw, TEX_CHANGE);
    m_texture_sb->setMin(1);
    m_texture_sb->setMax(100);
    m_texture_sb->setPos(25);

    m_auto_norm_cb = gui_env->addCheckBox(false, rect<s32>(30,145,210,175), 
                                          m_wndw, AUTO_NORM_CHECK_BOX, L"Calculate Normals");

    gui_env->addButton(rect<s32>(30, 180, 80, 230), m_wndw, ADD, L"", _(L"Add ControlPoint"))
        ->setImage(Editor::loadImg("img/dl_add.png"));
    gui_env->addButton(rect<s32>(95, 180, 145, 230), m_wndw, INSERT, L"", _(L"Insert ControlPoint"))
        ->setImage(Editor::loadImg("img/dl_insert.png"));
    gui_env->addButton(rect<s32>(160, 180, 210, 230), m_wndw, EXIT, L"", _(L"Finish Point Placing"))
        ->setImage(Editor::loadImg("img/dl_ready.png"));

    gui_env->addButton(rect<s32>(60, 245, 110, 295), m_wndw, CROSS_SECTION, L"", _(L"Editor road cross section"))
        ->setImage(Editor::loadImg("img/edit_crossroads_section.png"));
    gui_env->addButton(rect<s32>(130, 245, 180, 295), m_wndw, ATTACH_TO_DL, L"", _(L"Attach to DriveLine"))
        ->setImage(Editor::loadImg("img/create-quads.png"));

    m_spline_type_cb = gui_env->addComboBox(rect<s32>(30, 320, 150, 340), m_wndw);
    m_spline_type_cb->addItem(L"Bezier", 0);
    m_spline_type_cb->addItem(L"Catmull-Rom", 1);

    m_text_field = gui_env->addEditBox(L"RoadMesh_1",rect<s32>(30, 345, 150, 365),true,m_wndw,NAMEBOX);

    gui_env->addButton(rect<s32>(160, 320, 210, 365), m_wndw, CREATE, L"", _(L"Create New Road Mesh"))
        ->setImage(Editor::loadImg("img/road_create.png"));
        
    gui_env->addButton(rect<s32>(30, 380, 210, 430), m_wndw, CHECKLINE, L"CHECKLINE");

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
u32 RoadPanel::getTexWrapCount()
{
    return m_texture_sb->getPos();
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
    m_auto_norm_cb->setChecked(r->getAutoNorm());
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
