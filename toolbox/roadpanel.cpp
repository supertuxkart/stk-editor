#include "toolbox/roadpanel.hpp"
#include "editor.hpp"

#include "track.hpp"
#include "road/driveline.hpp"
#include "road/bezier.hpp"

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

    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    ISpline* spline = new Bezier(sm->getRootSceneNode(), sm, 0);
    IRoad* dl = new DriveLine(sm->getRootSceneNode(), sm, 0, spline);

    m_roads.insert(0, dl);
    Track::getTrack()->setActiveRoad(dl);
    Track::getTrack()->setSplineMode(false);

    m_insert = false;

} // init


// ----------------------------------------------------------------------------
void RoadPanel::create()
{
    Track::getTrack()->setState(Track::SELECT);
    stringw type = m_spline_type_cb->getItem(m_spline_type_cb->getSelected());
    IRoad* rm;

    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    ISpline* spline;

    if (type == L"Bezier")
    {
        spline = new Bezier(sm->getRootSceneNode(), sm, 0);
    }
    else spline = new TCR(sm->getRootSceneNode(), sm, 0);

    rm = new Road(sm->getRootSceneNode(), sm, 0, spline);

    m_cb->addItem(m_text_field->getText(),m_next_road_mesh_ID);
    m_cb->setSelected(m_next_road_mesh_ID);

    Track::getTrack()->roadBorn(rm, m_text_field->getText());

    m_next_road_mesh_ID++;
    stringw s = stringw(m_next_road_mesh_ID);
    m_text_field->setText((stringw("RoadMesh_") + s).c_str());

    m_roads.insert(m_next_road_mesh_ID-1, rm);
} // create

// ----------------------------------------------------------------------------
void RoadPanel::removeLastRoad()
{
    m_cb->removeItem(m_cb->getItemCount() - 1);
    m_next_road_mesh_ID--;
    m_roads.remove(m_next_road_mesh_ID);
    stringw s = stringw(m_next_road_mesh_ID);
    m_text_field->setText((stringw("RoadMesh_") + s).c_str());

    IRoad* r = m_roads.find(m_next_road_mesh_ID - 1)->getValue();
    Track::getTrack()->setActiveRoad(r);
    Track::getTrack()->setState(Track::SPLINE);

    m_cb->setSelected(m_next_road_mesh_ID-1);

} // removeLastRoad

// ----------------------------------------------------------------------------
void RoadPanel::restoreRoad(IRoad* road, stringw name)
{
    m_cb->addItem(name.c_str(), m_next_road_mesh_ID);
    m_cb->setSelected(m_next_road_mesh_ID);

    m_roads.insert(m_next_road_mesh_ID, road);

    Track::getTrack()->setActiveRoad(road);
    Track::getTrack()->setState(Track::SPLINE);

    m_next_road_mesh_ID++;
    stringw s = stringw(m_next_road_mesh_ID);
    m_text_field->setText((stringw("RoadMesh_") + s).c_str());
}  // restoreRoad

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
        create();
        break;
    default:
        break;
    }
} // btnDown


// ----------------------------------------------------------------------------
void RoadPanel::select()
{
    IRoad* r = m_roads.find(m_cb->getSelected())->getValue();
    Track::getTrack()->setActiveRoad(r);

} // select

void RoadPanel::scrollBarChanged()
{
    IRoad* r = m_roads.find(m_cb->getSelected())->getValue();
    r->setWidth(m_width_sb->getPos() / 10.0f);
    r->setDetail(m_detail_sb->getPos() / 100.0f);

    r->refresh();

} // scrollBarChanged
