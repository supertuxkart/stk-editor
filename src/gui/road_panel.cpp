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
    IGUIFont* font = gui_env->getFont(L"font/font1.png");
    m_wndw->setRelativePosition(position2di(0, 50));
    gui_env->addStaticText(L"DriveLine:", rect<s32>(10, 10, 200, 30),
        false, false, m_wndw, -1, false)->setOverrideFont(font);


    m_cb = gui_env->addComboBox(rect<s32>(30, 50, 210, 70), m_wndw, DL_SELECT);
    m_cb->addItem(L"DriveLine", 0);

    gui_env->addStaticText(L"Width:", rect<s32>(30, 77, 90, 92), false, false, m_wndw);
    m_width_sb = gui_env->addScrollBar(true, rect<s32>(100, 80, 210, 90), m_wndw,DL_WIDTH);
    m_width_sb->setMin(1);
    m_width_sb->setMax(100);
    m_width_sb->setPos(40);

    gui_env->addStaticText(L"Detail:", rect<s32>(30, 92, 90, 117), false, false, m_wndw);
    m_detail_sb = gui_env->addScrollBar(true, rect<s32>(100, 95, 210, 105), m_wndw, DL_DETAIL);
    m_detail_sb->setMin(1);
    m_detail_sb->setMax(100);
    m_detail_sb->setPos(25);

    gui_env->addButton(rect<s32>(30, 130, 80, 180), m_wndw, DL_ADD)
        ->setImage(Editor::loadImg("img/dl_add.png"));
    gui_env->addButton(rect<s32>(95, 130, 145, 180), m_wndw, DL_INSERT)
        ->setImage(Editor::loadImg("img/dl_insert.png"));
    gui_env->addButton(rect<s32>(160, 130, 210, 180), m_wndw, DL_EXIT)
        ->setImage(Editor::loadImg("img/dl_ready.png"));

    m_spline_type_cb = gui_env->addComboBox(rect<s32>(30, 200, 150, 220), m_wndw);
    m_spline_type_cb->addItem(L"Bezier", 0);
    m_spline_type_cb->addItem(L"Catmull-Rom", 1);

    m_text_field = gui_env->addEditBox(L"RoadMesh_1",rect<s32>(30, 225, 150, 245),true,m_wndw);

    gui_env->addButton(rect<s32>(160, 200, 210, 245), m_wndw, DL_CREATE);

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
    case DL_ADD:
        m_insert = false;
        break;
    case DL_INSERT:
        m_insert = true;
        break;
    case DL_CREATE:
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
void RoadPanel::scrollBarChanged()
{
    //r->setWidth(m_width_sb->getPos() / 10.0f);
    //r->setDetail(m_detail_sb->getPos() / 100.0f);
    //
    //r->refresh();

} // scrollBarChanged



//----------------------------------------------------------------------------
void RoadPanel::updateRoadList()
{
    Track* t = Viewport::get()->getTrack();
    if (t)
    {
        IRoad* r;
        map<unsigned int, IRoad*>* road_list = t->getRoadList();
        m_cb->clear();

        for (int i = 0; i < road_list->size(); i++)
        {
            r = (*road_list)[i];
            stringw name = r->getName();
            m_cb->addItem(name.c_str(), i);
        }        

        stringw s = stringw(road_list->size());
        m_text_field->setText((stringw("RoadMesh_") + s).c_str());
        m_cb->setSelected(road_list->size()-1);
    }
}


//----------------------------------------------------------------------------
stringw RoadPanel::getNextRoadType()
{
    return m_spline_type_cb->getItem(m_spline_type_cb->getSelected());
} // getNextRoadType

