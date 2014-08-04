#include "gui/road_cross_section_wndw.hpp"

#include "editor.hpp"
#include "mesh/road.hpp"
#include "viewport/viewport.hpp"

RoadCrossSectionWndw* RoadCrossSectionWndw::m_self = 0;

// ----------------------------------------------------------------------------
void RoadCrossSectionWndw::init()
{
    IGUIEnvironment* gui_env = Editor::getEditor()->getGUIEnv();
    m_smgr                   = Editor::getEditor()->getSceneManager();
    m_driver                 = Editor::getEditor()->getVideoDriver();

    dimension2du ss = Editor::getEditor()->getScreenSize();

    rect<s32> frame = rect<s32>((s32)(ss.Width / 2.0f - 300),
        (s32)(ss.Height / 2.0f - 250),
        (s32)(ss.Width / 2.0f + 300),
        (s32)(ss.Height / 2.0f + 250));

    ICameraSceneNode* c = m_smgr->getActiveCamera();
    m_cam = m_smgr->addCameraSceneNode(0, vector3df(m_offset - 1, 0, 5), 
                                          vector3df(m_offset - 1, 0, 0));
    m_cam->setID(1);

    matrix4 mat;
    f32 nv = m_cam->getNearValue();
    f32 fv = m_cam->getFarValue();
    f32 hVol = 10 * ss.Height / ss.Width;
    mat.buildProjectionMatrixOrthoLH(10, hVol, nv, fv);
    m_cam->setProjectionMatrix(mat, true);

    m_smgr->setActiveCamera(c);
    m_nodes = 0;
    m_visible = false;
} // init

// ----------------------------------------------------------------------------
RoadCrossSectionWndw* RoadCrossSectionWndw::get()
{
    if (m_self != 0) return m_self;

    m_self = new RoadCrossSectionWndw();
    m_self->init();
    return m_self;
} // get

// ----------------------------------------------------------------------------
void RoadCrossSectionWndw::show(Road* r)
{
    m_smgr->setActiveCamera(m_cam);
    m_road = r;    
    array<vector2df> points = m_road->getCrossSectionArray();
    m_node_num = points.size();

    m_nodes = new ISceneNode*[m_node_num];
    vector3df pos = vector3df(0);

    for (u32 i = 0; i < m_node_num; i++)
    {
        m_nodes[i] = m_smgr->addSphereSceneNode(0.05);
        pos.X = points[i].X + m_offset;
        pos.Y = points[i].Y;
        m_nodes[i]->setPosition(pos);
        (m_nodes[i]->getMaterial(0)).DiffuseColor = SColor(125, 255, 0, 0);
        (m_nodes[i]->getMaterial(0)).Lighting     = false;
    }
    m_visible = true;
} // show

// ----------------------------------------------------------------------------
void RoadCrossSectionWndw::hide()
{
    array<vector2df> points;
    vector2df pos;
    for (int i = 0; i < m_node_num; i++)
    {
        pos.X = m_nodes[i]->getPosition().X - m_offset;
        pos.Y = m_nodes[i]->getPosition().Y;
        points.push_back(pos);
    }
    m_road->setCrossSection(points);
    m_road->refresh();

    for (u32 i = 0; i < m_node_num; i++)
        m_nodes[i]->remove();
    delete m_nodes;
    m_nodes = 0;
    m_road  = 0;

    m_visible = false;
} // hide

// ----------------------------------------------------------------------------
void RoadCrossSectionWndw::render()
{
    if (m_visible)
    {
        for (int i = 0; i < m_node_num - 1; i++)
            m_driver->draw3DLine(m_nodes[i]->getPosition(), m_nodes[i+1]->getPosition(),SColor(125,255,0,0));
        m_driver->draw3DLine(m_nodes[m_node_num - 1]->getPosition(), m_nodes[0]->getPosition(), SColor(125, 255, 0, 0));
    }
} // render
