#include "mesh/iroad.hpp"

#include "editor.hpp"

#include "spline/tcr.hpp"
#include "spline/bezier.hpp"

#include <iostream>

// ----------------------------------------------------------------------------
IRoad::IRoad(ISceneNode* parent, ISceneManager* mgr, s32 id, ISpline* s, stringw n)
                                                       :ISceneNode(parent, mgr, id)
{
    setName(n);
    m_spline            = s;
    m_mesh.indices      = 0;
    m_mesh.vertices     = 0;
    m_mesh.vertex_count = 0;
    m_mesh.quad_count   = 0;
    m_detail            = 0.25f;
    m_width             = 4.0f;
    m_width_vert_num    = 12;

    m_material.Wireframe       = true;
    m_material.Lighting        = false;
    m_material.BackfaceCulling = false;

    setAutomaticCulling(EAC_OFF);

} // IRoad

// ----------------------------------------------------------------------------
IRoad::IRoad(ISceneNode* parent, ISceneManager* mgr, s32 id, FILE* fp)
                                          :ISceneNode(parent, mgr, id)
{
    u8 size;
    c8* cc;
    fread(&size, sizeof(u8), 1, fp);
    cc = new c8[size];
    fread(cc, sizeof(c8), size, fp);
    setName(cc);
    delete cc;


    fread(&m_width_vert_num, sizeof(u32), 1, fp);
    fread(&m_detail, sizeof(f32), 1, fp);
    fread(&m_width, sizeof(f32), 1, fp);

    stringw type;
    wchar_t* c;
    fread(&size, sizeof(u8), 1, fp);
    c = new wchar_t[size];
    fread(c, sizeof(wchar_t), size, fp);
    type = c;
    delete c;

    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    if (type == L"TCR")
        m_spline = new TCR(sm->getRootSceneNode(), sm, 0, fp);
    else m_spline = new Bezier(sm->getRootSceneNode(), sm, 0, fp);

    m_material.Wireframe = true;
    m_material.Lighting = false;
    m_material.BackfaceCulling = false;

    setAutomaticCulling(EAC_OFF);

} // IRoad

// ----------------------------------------------------------------------------
void IRoad::clear()
{
    m_spline->clear();
    m_spline->remove();
}

// ----------------------------------------------------------------------------
void IRoad::save(FILE* fp)
{
    u32 size = strlen(getName()) + 1;
    fwrite(&size, sizeof(u8), 1, fp);
    fwrite(getName(), sizeof(c8), size, fp);

    fwrite(&m_width_vert_num, sizeof(u32), 1, fp);
    fwrite(&m_detail, sizeof(f32), 1, fp);
    fwrite(&m_width, sizeof(f32), 1, fp);

    m_spline->save(fp);
} // save

// ----------------------------------------------------------------------------
void IRoad::OnRegisterSceneNode()
{
    if (IsVisible)
        SceneManager->registerNodeForRendering(this);

    ISceneNode::OnRegisterSceneNode();

} // OnRegisterSceneNode


