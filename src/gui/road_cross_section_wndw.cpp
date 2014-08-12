#include "gui/road_cross_section_wndw.hpp"

#include "editor.hpp"
#include "mesh/road.hpp"

RoadCrossSectionWndw* RoadCrossSectionWndw::m_self = 0;

// ----------------------------------------------------------------------------
void RoadCrossSectionWndw::init()
{
    m_sym_mode = false;
    m_allign   = false;
    m_grid_on  = true;
    m_grid     = 0.25;
    m_rt       = 0;
    IGUIEnvironment* gui_env = Editor::getEditor()->getGUIEnv();
    m_smgr                   = Editor::getEditor()->getSceneManager();
    m_driver                 = Editor::getEditor()->getVideoDriver();

    dimension2du ss = Editor::getEditor()->getScreenSize();

    ICameraSceneNode* c = m_smgr->getActiveCamera();
    m_cam = m_smgr->addCameraSceneNode(0, vector3df(m_offset - 1, 0, 5),
                                          vector3df(m_offset - 1, 0, 0));
    m_cam->setID(1);

    matrix4 mat;
    f32 nv = m_cam->getNearValue();
    f32 fv = m_cam->getFarValue();
    f32 hVol = 10.0f * ss.Height / ss.Width;
    mat.buildProjectionMatrixOrthoLH(10, hVol, nv, fv);
    m_cam->setProjectionMatrix(mat, true);

    m_smgr->setActiveCamera(c);
    m_nodes = 0;
    m_visible = false;

    m_center_node = m_smgr->addSphereSceneNode(0.02f);
    m_center_node->setID(1);
    m_center_node->setPosition(vector3df((f32)m_offset, 0, 0));
    m_center_node->getMaterial(0).DiffuseColor = SColor(255, 0, 255, 0);
    m_center_node->getMaterial(0).AmbientColor = SColor(255, 0, 255, 0);
    m_center_node->setVisible(false);

    f32 dx = (ss.Width - 300 - 8 * 50) / 9.0f;
    f32 x = 25 + dx;
    s32 h = ss.Height;

    m_gof    = gui_env->addButton(rect<s32>((s32)x,h - 100, (s32)x+50, h-50)); x += dx + 50;
    m_gp     = gui_env->addButton(rect<s32>((s32)x,h - 100, (s32)x+50, h-50)); x += dx + 50;
    m_gm     = gui_env->addButton(rect<s32>((s32)x,h - 100, (s32)x+50, h-50)); x += dx + 50;
    m_pp     = gui_env->addButton(rect<s32>((s32)x,h - 100, (s32)x+50, h-50)); x += dx + 50;
    m_pm     = gui_env->addButton(rect<s32>((s32)x,h - 100, (s32)x+50, h-50)); x += dx + 50;
    m_sym    = gui_env->addButton(rect<s32>((s32)x,h - 100, (s32)x+50, h-50)); x += dx + 50;
    m_ok     = gui_env->addButton(rect<s32>((s32)x,h - 100, (s32)x+50, h-50)); x += dx + 50;
    m_cancel = gui_env->addButton(rect<s32>((s32)x,h - 100, (s32)x+50, h-50)); x += dx + 50;

    m_gof    ->setImage(Editor::loadImg("img/grid-on_align-off.png"));
    m_gp     ->setImage(Editor::loadImg("img/grid-plus.png"));
    m_gm     ->setImage(Editor::loadImg("img/grid-minus.png"));
    m_pp     ->setImage(Editor::loadImg("img/points-plus.png"));
    m_pm     ->setImage(Editor::loadImg("img/points-minus.png"));
    m_sym    ->setImage(Editor::loadImg("img/symm_off.png"));
    m_ok     ->setImage(Editor::loadImg("img/qsave.png"));
    m_cancel ->setImage(Editor::loadImg("img/qcancel.png"));

    m_gof    ->setID(GRID_ON_OFF);
    m_gp     ->setID(GRID_P     );
    m_gm     ->setID(GRID_M     );
    m_pp     ->setID(POINT_P    );
    m_pm     ->setID(POINT_M    );
    m_sym    ->setID(SYM_ON_OFF );
    m_ok     ->setID(OK         );
    m_cancel ->setID(CANCEL     );    

    m_gof    ->setVisible(false);
    m_gp     ->setVisible(false);
    m_gm     ->setVisible(false);
    m_pp     ->setVisible(false);
    m_pm     ->setVisible(false);
    m_sym    ->setVisible(false);
    m_ok     ->setVisible(false);
    m_cancel ->setVisible(false);

    m_gof    ->setToolTipText(_(L"Grid on alignment on"));
    m_gp     ->setToolTipText(_(L"Increase grid density"));
    m_gm     ->setToolTipText(_(L"Decrease grid density"));
    m_pp     ->setToolTipText(_(L"Add 4 points"));
    m_pm     ->setToolTipText(_(L"Remove 4 points"));
    m_sym    ->setToolTipText(_(L"Click to turn on symmetry"));
    m_ok     ->setToolTipText(_(L"Save and Leave"));
    m_cancel ->setToolTipText(_(L"Leave without save"));

} // init

// ----------------------------------------------------------------------------
void RoadCrossSectionWndw::setPointNum(u32 n)
{
    if (n < 4) return;
    if (m_nodes)
    {
        for (u32 i = 0; i < m_node_num; i++)
            m_nodes[i]->remove();
        delete m_nodes;
        m_nodes = 0;
    }
    m_node_num = n;
    createNodesFromPoints(m_road->genStandardCrossSection(n));
} // setPointNum

// ----------------------------------------------------------------------------
void RoadCrossSectionWndw::createNodesFromPoints(array<vector2df> points)
{
    m_nodes = new ISceneNode*[m_node_num];
    vector3df pos = vector3df(0);

    for (u32 i = 0; i < m_node_num; i++)
    {
        m_nodes[i] = m_smgr->addSphereSceneNode(0.05f);
        pos.X = points[i].X + m_offset;
        pos.Y = points[i].Y;
        m_nodes[i]->setPosition(pos);
        (m_nodes[i]->getMaterial(0)).DiffuseColor = SColor(255, 255, 0, 0);
        (m_nodes[i]->getMaterial(0)).AmbientColor = SColor(255, 255, 0, 0);
    }
} // createNodesFromPoints

// ----------------------------------------------------------------------------
void RoadCrossSectionWndw::drawGrid()
{
    const s32 sx = 4;
    if (m_grid_on)
    {
        SMaterial mat;
        mat.Lighting = false;
        m_driver->setMaterial(mat);
        for (s32 s = 1; s > -2; s -= 2)
        {
            for (f32 dy = 0; dy < sx; dy += m_grid)
            {
                bool   cl = (s32)(dy * 100) % 100 == 0;
                SColor col = SColor(255, (1 - cl) * 100, 100 + 35 * cl, 100 + 60 * cl);
                vector3df start = vector3df(-sx + m_offset, s * dy, -0.01f);
                vector3df end = vector3df(sx + m_offset, s * dy, -0.01f);
                m_driver->draw3DLine(start, end, col);

                start = vector3df((f32)s * dy + m_offset, (f32)-sx, -0.01f);
                end   = vector3df((f32)s * dy + m_offset, (f32) sx, -0.01f);
                m_driver->draw3DLine(start, end, col);
            } // dy for
        } // sign for
    } // m_grid_on
} // render

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
    m_sym_mode = false;
    m_smgr->setActiveCamera(m_cam);
    m_road = r;
    array<vector2df> points = m_road->getCrossSectionArray();
    m_node_num = points.size();
    createNodesFromPoints(points);

    m_visible = true;
    m_center_node->setVisible(true);

    m_gof    ->setVisible(true);
    m_gp     ->setVisible(true);
    m_gm     ->setVisible(true);
    m_pp     ->setVisible(true);
    m_pm     ->setVisible(true);
    m_sym    ->setVisible(true);
    m_ok     ->setVisible(true);
    m_cancel ->setVisible(true);
} // show

// ----------------------------------------------------------------------------
void RoadCrossSectionWndw::hide(bool apply_mod)
{
    if (apply_mod)
    {
        array<vector2df> points = genCrossSectionArray();
        m_road->setCrossSection(points);
        m_road->refresh();
    } // apply modifications

    for (u32 i = 0; i < m_node_num; i++)
        m_nodes[i]->remove();
    delete m_nodes;
    m_nodes = 0;
    m_road  = 0;

    m_visible = false;
    m_center_node->setVisible(false);

    m_gof    ->setVisible(false);
    m_gp     ->setVisible(false);
    m_gm     ->setVisible(false);
    m_pp     ->setVisible(false);
    m_pm     ->setVisible(false);
    m_sym    ->setVisible(false);
    m_ok     ->setVisible(false);
    m_cancel ->setVisible(false);
} // hide

// ----------------------------------------------------------------------------
void RoadCrossSectionWndw::render()
{
    if (m_visible)
    {
        SMaterial mat;
        mat.Lighting = false;
        m_driver->setMaterial(mat);
        for (u32 i = 0; i < m_node_num - 1; i++)
            m_driver->draw3DLine(m_nodes[i]->getPosition(),
                                 m_nodes[i+1]->getPosition(),SColor(255,255,0,0));

        m_driver->draw3DLine(m_nodes[m_node_num - 1]->getPosition(),
                             m_nodes[0]->getPosition(), SColor(255, 255, 0, 0));
        drawGrid();
    }
} // render

// ----------------------------------------------------------------------------
void RoadCrossSectionWndw::animate(u32 dt, bool dirty)
{
    // Symetry
    if (m_sym_mode && dirty)
    {
        for (u32 j = 0; j < 2; j++)
        {
            s32 s = 1 - 2 * j;
            for (u32 i = 0; i < m_node_num / 4; i++)
            {
                vector3df pos = m_nodes[m_node_num / 2 - s * i - 1 + j]->getPosition();
                pos.X = 2 * m_offset - pos.X;
                m_nodes[j * (m_node_num-1) + s * i]->setPosition(pos);
            }
            s = -1;
        } // j = 0, 1
    }  // m_sym_mode

    // Grid
    if (m_allign)
    {
        m_rt -= dt;
        if (m_rt < 0)
        {
            m_rt = 500;
            for (u32 i = 0; i < m_node_num; i++)
            {
                vector3df pos = m_nodes[i]->getPosition();
                f32 x = fabs(pos.X);
                f32 y = fabs(pos.Y);

                u32 m = (s32)(x * 1000) % (s32)(m_grid * 1000);
                x -= m / 1000.0f - m_grid * (m > m_grid * 500);
                pos.X = x * pos.X / fabs(pos.X);

                m = (s32)(y * 1000) % (s32)(m_grid * 1000);
                y -= m / 1000.0f - m_grid * (m > m_grid * 500);
                pos.Y = y * pos.Y / fabs(pos.Y);

                m_nodes[i]->setPosition(pos);
            } // for - nodes
        } // remaining time < 0
    } // grid on
} // animate

// ----------------------------------------------------------------------------
void RoadCrossSectionWndw::buttonClicked(u32 id)
{
    switch (id)
    {
    case SYM_ON_OFF:
        m_sym_mode = !m_sym_mode;        
        if (m_sym_mode)
        {
            m_sym->setImage(Editor::loadImg("img/symm_on.png"));
            m_sym->setToolTipText(_(L"Click to turn off symmetry"));
        }
        else
        {
            m_sym->setImage(Editor::loadImg("img/symm_off.png"));
            m_sym->setToolTipText(_(L"Click to turn on symmetry"));
        }        
        for (u32 j = 0; j < 2; j++)
        {
            for (u32 i = 0; i < m_node_num / 4; i++)
            {
                m_nodes[i + j * 3 * m_node_num / 4]->getMaterial(0).DiffuseColor =
                    SColor(255, m_sym_mode ? 0 : 255, 0, m_sym_mode ? 255 : 0);
                m_nodes[i + j * 3 * m_node_num / 4]->getMaterial(0).AmbientColor =
                    SColor(255, m_sym_mode ? 0 : 255, 0, m_sym_mode ? 255 : 0);
            }
        }
        return;
    case POINT_M:
        setPointNum(m_node_num - 4);
        return;
    case POINT_P:
        setPointNum(m_node_num + 4);
        return;
    case GRID_ON_OFF:
        if (!m_grid_on) 
        { 
            m_grid_on = true; 
            m_gof->setImage(Editor::loadImg("img/grid-on_align-off.png"));
        } // grid on
        else
        {
            if (!m_allign) 
            { 
                m_allign = true; 
                m_gof->setImage(Editor::loadImg("img/grid-on_align-on.png"));
                m_gof->setToolTipText(_(L"Grid off alignment off"));
            }
            else 
            { 
                m_allign = false; m_grid_on = false; 
                m_gof->setImage(Editor::loadImg("img/grid-off_align-off.png"));
                m_gof->setToolTipText(_(L"Grid on alignment off"));
            } // allign on
        } // !grid on
        return;
    case GRID_M:
        m_grid *= 2;
        if (m_grid > 1) m_grid = 1;
        return;
    case GRID_P:
        m_grid /= 2;
        if (m_grid < 0.0625) m_grid = 0.0625;
        return;
    default:
        return;
    } // switch (id)
} // buttonClicked

// ----------------------------------------------------------------------------
void RoadCrossSectionWndw::reallocate()
{
    dimension2du ss = Editor::getEditor()->getScreenSize();
    f32          dx = (ss.Width - 300 - 8 * 50) / 9.0f;
    f32          x  = 25 + dx;
    s32          h  = ss.Height;

    m_gof    ->setRelativePosition(position2di((s32)x,h - 100)); x += dx + 50;
    m_gp     ->setRelativePosition(position2di((s32)x,h - 100)); x += dx + 50;
    m_gm     ->setRelativePosition(position2di((s32)x,h - 100)); x += dx + 50;
    m_pp     ->setRelativePosition(position2di((s32)x,h - 100)); x += dx + 50;
    m_pm     ->setRelativePosition(position2di((s32)x,h - 100)); x += dx + 50;
    m_sym    ->setRelativePosition(position2di((s32)x,h - 100)); x += dx + 50;
    m_ok     ->setRelativePosition(position2di((s32)x,h - 100)); x += dx + 50;
    m_cancel ->setRelativePosition(position2di((s32)x,h - 100)); x += dx + 50;
} // reallocate

// ----------------------------------------------------------------------------
array<vector2df> RoadCrossSectionWndw::genCrossSectionArray()
{
    array<vector2df> points;
    vector2df pos;
    for (u32 i = 0; i < m_node_num; i++)
    {
        pos.X = m_nodes[i]->getPosition().X - m_offset;
        pos.Y = m_nodes[i]->getPosition().Y;
        points.push_back(pos);
    }
    return points;
} // genCrossSectionArray
