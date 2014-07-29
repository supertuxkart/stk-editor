#include "mesh/iroad.hpp"

#include "editor.hpp"

#include "spline/tcr.hpp"
#include "spline/bezier.hpp"

#include <iostream>

#define MAX_DETAIL 100       
#define MAX_WIDTH 200
#define MAX_WVT 20

// ----------------------------------------------------------------------------
IRoad::IRoad(ISceneNode* parent, ISceneManager* mgr, s32 id, ISpline* s, stringw n)
                                                       :ISceneNode(parent, mgr, id)
{
    setName(n);
    m_spline            = s;
    m_detail            = 0.25f;
    m_width             = 4.0f;
    m_width_vert_num    = 12;
    m_tex_warp_count    = 10;

    setAutomaticCulling(EAC_OFF);

} // IRoad

// ----------------------------------------------------------------------------
IRoad::IRoad(ISceneNode* parent, ISceneManager* mgr, s32 id, FILE* fp)
                                          :ISceneNode(parent, mgr, id)
{
    u8 size;
    c8* cc;
    fread(&size, sizeof(u8), 1, fp);
    if (size > 40)
    {
        m_valid = false;
        return;
    }
    cc = new c8[size];
    fread(cc, sizeof(c8), size, fp);
    setName(cc);
    delete[] cc;

    fread(&m_width_vert_num, sizeof(u32), 1, fp);
    fread(&m_detail, sizeof(f32), 1, fp);
    fread(&m_width, sizeof(f32), 1, fp);
    fread(&m_tex_warp_count, sizeof(u32), 1, fp);

    if (m_width_vert_num < 0 || m_detail < 0 || m_width < 0
        || m_width_vert_num > MAX_WVT ||m_detail > MAX_DETAIL ||m_width > MAX_WIDTH)
    {
        m_valid = false;
        return;
    }

    stringw type;
    wchar_t* c;
    fread(&size, sizeof(u8), 1, fp);
    c = new wchar_t[size];
    fread(c, sizeof(wchar_t), size, fp);
    type = c;
    delete[] c;

    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    if (type == L"TCR")
        m_spline = new TCR(sm->getRootSceneNode(), sm, 0, fp);
    else m_spline = new Bezier(sm->getRootSceneNode(), sm, 0, fp);
    setAutomaticCulling(EAC_OFF);
    m_valid = m_spline->isValid();
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
    fwrite(&m_tex_warp_count, sizeof(u32), 1, fp);

    m_spline->save(fp);
    textureExport(fp);
} // save

// ----------------------------------------------------------------------------
void IRoad::OnRegisterSceneNode()
{
    if (IsVisible)
        SceneManager->registerNodeForRendering(this);

    ISceneNode::OnRegisterSceneNode();

} // OnRegisterSceneNode


