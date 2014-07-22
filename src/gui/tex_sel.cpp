#include "gui/tex_sel.hpp"

#include "gui/isubscriber.hpp"

#include "editor.hpp"

#include <iostream>
#include <assert.h>

TexSel* TexSel::m_self = 0;

// ----------------------------------------------------------------------------
void TexSel::init()
{
    IGUIEnvironment* gui_env = Editor::getEditor()->getGUIEnv();

    dimension2du ss = Editor::getEditor()->getScreenSize();

    m_wndw = gui_env->addWindow(rect<s32>(ss.Width-500, 50, ss.Width-250, ss.Height),
                                false, _(L"Textures"), 0, 0);

    m_search_field = gui_env->addEditBox(L"", rect<s32>(40, 40, 210, 60), 
                                                        true, m_wndw, SEARCH_BOX);
    m_selected_page = 0;

    m_wndw->getCloseButton()->setVisible(false);

    m_next = gui_env->addButton(rect<s32>(190, ss.Height - 78, 240, ss.Height - 58),
        m_wndw, FIRST_BTN_ID + 1);

    m_prev = gui_env->addButton(rect<s32>(10, ss.Height - 78, 60, ss.Height - 58),
        m_wndw, FIRST_BTN_ID);

    m_cancel = gui_env->addButton(rect<s32>(95, ss.Height - 78, 155, ss.Height - 58),
        m_wndw, FIRST_BTN_ID + 2, _(L"Cancel"));

    loadTextures();
    initButtons();
    bindTexturesToButton(0);
    hide();
    
} // init

// ----------------------------------------------------------------------------
bool TexSel::texContainsString(ITexture* tex, stringw str)
{
    stringc n = tex->getName();
    u32 ix;
    ix = n.findLast('/');
    n = n.subString(ix + 1, n.size() - ix - 1);
    if (n.find(str.c_str(), 0) != -1) return true;
    else return false;
} // texContainsString

// ----------------------------------------------------------------------------
void TexSel::bindTexturesToButton(u32 page)
{
    list<ITexture*>::Iterator it;
    
    it = m_tex_list.begin();
    stringw s = m_search_field->getText();

    for (u32 i = 0; i < page * m_btn_num;)
    {
        it++;
        assert(*it!=NULL);
        if (s == "" || texContainsString(*it,s)) i++;
    }

    for (int i = 0; i < m_btn_num;)
    {
        if (it!=m_tex_list.end())
        {
            if (s=="" || texContainsString(*it, s))
            {
                m_btn_table[i].first->setVisible(true);
                m_btn_table[i].first->setImage(*it);
                m_btn_table[i].second = *it;
                i++;
            }
            it++;
        }
        else 
        {
            m_btn_table[i].first->setVisible(false);
            i++;
        }
    }

} // bindTexturesToButton

// ----------------------------------------------------------------------------
void TexSel::loadTextures()
{
    IFileSystem* file_system = Editor::getEditor()->getDevice()->getFileSystem();
    IFileArchive* dir = Editor::getEditor()->getTexDir();
    ITexture* t;
    const IFileList* file_list = dir->getFileList();
    for (unsigned int i = 0; i < file_list->getFileCount(); i++)
    {
        if ((t = Editor::getEditor()->getVideoDriver()
                                    ->getTexture(file_list->getFullFileName(i))))
        {
            m_tex_list.push_back(t);
        }
    }

} // loadTextures


// ----------------------------------------------------------------------------
void TexSel::initButtons()
{
    IGUIEnvironment* gui_env = Editor::getEditor()->getGUIEnv();
    dimension2du ss = Editor::getEditor()->getScreenSize();
    if (ss.Height < 180)
    {
        m_btn_num = 0;
        return;
    }
    else m_btn_num = (ss.Height - 180) / 60 * 4;
    m_btn_table = new std::pair<IGUIButton*,ITexture*>[m_btn_num];

    for (int i = 0; i < m_btn_num / 4; i++)
    {
        m_btn_table[4 * i].first = gui_env->addButton
            (rect<s32>(10, i * 60 + 80, 60, i * 60 + 130), m_wndw, FIRST_TEX_BTN_ID + 4 * i);

        m_btn_table[4 * i + 1].first = gui_env->addButton
            (rect<s32>(70, i * 60 + 80, 120, i * 60 + 130), m_wndw, FIRST_TEX_BTN_ID + 4 * i + 1);

        m_btn_table[4 * i + 2].first = gui_env->addButton
            (rect<s32>(130, i * 60 + 80, 180, i * 60 + 130), m_wndw, FIRST_TEX_BTN_ID + 4 * i + 2);

        m_btn_table[4 * i + 3].first = gui_env->addButton
            (rect<s32>(190, i * 60 + 80, 240, i * 60 + 130), m_wndw, FIRST_TEX_BTN_ID + 4 * i + 3);
    }

    m_next->setRelativePosition(rect<s32>(190, ss.Height - 78, 240, ss.Height - 58));
    m_prev->setRelativePosition(rect<s32>(10, ss.Height - 78, 60, ss.Height - 58));
    m_cancel->setRelativePosition(rect<s32>(95, ss.Height - 78, 155, ss.Height - 58));

} // initButtons

// ----------------------------------------------------------------------------
u32 TexSel::properTexNum()
{
    u32 ptnum = 0;
    stringw s = m_search_field->getText();

    if (s == L"") return m_tex_list.size();

    list<ITexture*>::Iterator it;
    for (it = m_tex_list.begin(); it != m_tex_list.end(); it++)
    {
        if (texContainsString(*it, s)) ptnum++;
    }
    return ptnum;
} // properTexNum

// ----------------------------------------------------------------------------
TexSel* TexSel::getTexSel()
{
    if (m_self != 0) return m_self;

    m_self = new TexSel();
    m_self->init();
    return m_self;
} // getToolBox


// ----------------------------------------------------------------------------
void TexSel::btnClicked(u32 id)
{
    s32 ix = id - FIRST_TEX_BTN_ID;

    if (ix > 0)
    {
        notify(ix);
        hide();
    }
    else if (ix == -3)
    {
        if (m_selected_page > 0) m_selected_page--;
        bindTexturesToButton(m_selected_page);
    }
    else if (ix == -2)
    {
        m_selected_page++;
        if (m_selected_page > properTexNum() / m_btn_num) m_selected_page -= 1;
        bindTexturesToButton(m_selected_page);
    }
    else if (ix == -1)
    {
        m_bsubs.clear();
        m_subs.clear();
        hide();
    }

} // btnClicked

// ----------------------------------------------------------------------------
void TexSel::notify(u32 id)
{
    for (list<IGUIButton*>::Iterator it = m_bsubs.begin(); it != m_bsubs.end(); it++)
        (*it)->setImage(m_btn_table[id].second);

    for (list<ISubscriber*>::Iterator it = m_subs.begin(); it != m_subs.end(); it++)
        (*it)->notify(m_btn_table[id].second);
    m_bsubs.clear();
    m_subs.clear();

} // notify

// ----------------------------------------------------------------------------
void TexSel::searchFieldDirty()
{
    m_selected_page = 0;
    bindTexturesToButton(0);
} // searchFieldDirty

void TexSel::reallocate()
{
    dimension2du ss = Editor::getEditor()->getScreenSize();
    m_wndw->setMinSize(dimension2du(250, ss.Height - 50));
    m_wndw->setRelativePosition(position2di(ss.Width - 500, 50));

    int new_btn_num;
    if (ss.Height<180) new_btn_num = 0;
    else new_btn_num = (ss.Height - 180) / 60 * 4;
    if (new_btn_num != m_btn_num)
    {
        if (m_btn_num>0)
        {
            for (int i = 0; i < m_btn_num; i++)
                m_btn_table[i].first->remove();
            delete[] m_btn_table;
        }
        initButtons();
        bindTexturesToButton(0);
        m_next->setID(FIRST_BTN_ID + m_btn_num + 1);
        m_prev->setID(FIRST_BTN_ID + m_btn_num);
    }

} // reallocate

