#include "gui/tex_sel.hpp"

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
                                false, L"Textures", 0, 0);

    m_search_field = gui_env->addEditBox(L"", rect<s32>(40, 40, 210, 60), 
                                                        true, m_wndw, SEARCH_BOX);

    m_selected_ix   = 0;
    m_clicked       = false;
    m_selected_page = 0;

    loadTextures();
    initButtons();
    bindTexturesToButton(0);
    
} // init

// ----------------------------------------------------------------------------
void TexSel::bindTexturesToButton(u32 page)
{
    list<ITexture*>::Iterator it;

    it = m_tex_list.begin();

    for (int i = 0; i < page * m_btn_num; i++)
    {
        it++;
        assert(*it!=NULL);
    }

    for (int i = 0; i < m_btn_num; i++)
    {
        if (it!=m_tex_list.end())
        {
            m_btn_table[i]->setVisible(true);
            m_btn_table[i]->setImage(*it);
            it++;
        }
        else m_btn_table[i]->setVisible(false);
    }

} // bindTexturesToButton

// ----------------------------------------------------------------------------
void TexSel::loadTextures()
{
    IFileSystem* file_system = Editor::getEditor()->getDevice()->getFileSystem();
    path wd = file_system->getWorkingDirectory();
    IFileArchive* dir = 0;
    stringw dir_path = "/libraries/textures";
    if (!file_system->addFileArchive(wd+dir_path, true, false, EFAT_FOLDER, "", &dir))
    {
        std::cerr << "The textures directory could not be found. "
            "That's sad :( ";
        exit(-1);
    }

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
    m_btn_num = (ss.Height - 180) / 60 * 4;
    m_btn_table = new IGUIButton*[m_btn_num];

    for (int i = 0; i < m_btn_num / 4; i++)
    {
        m_btn_table[4 * i] = gui_env->addButton
            (rect<s32>(10, i * 60 + 80, 60, i * 60 + 130), m_wndw, FIRST_BTN_ID + 4 * i);

        m_btn_table[4 * i + 1] = gui_env->addButton
            (rect<s32>(70, i * 60 + 80, 120, i * 60 + 130), m_wndw, FIRST_BTN_ID + 4 * i + 1);

        m_btn_table[4 * i + 2] = gui_env->addButton
            (rect<s32>(130, i * 60 + 80, 180, i * 60 + 130), m_wndw, FIRST_BTN_ID + 4 * i + 2);

        m_btn_table[4 * i + 3] = gui_env->addButton
            (rect<s32>(190, i * 60 + 80, 240, i * 60 + 130), m_wndw, FIRST_BTN_ID + 4 * i + 3);
    }

    m_next = gui_env->addButton(rect<s32>(190, ss.Height - 78, 240, ss.Height - 58),
        m_wndw, FIRST_BTN_ID + m_btn_num + 1);

    m_prev = gui_env->addButton(rect<s32>(10, ss.Height - 78, 60, ss.Height - 58),
        m_wndw, FIRST_BTN_ID + m_btn_num);

} // initButtons


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
    u32 ix = id - FIRST_BTN_ID;

    if (ix < m_btn_num)
    {
        m_selected_ix = ix;
        m_clicked     = true;
    }
    else if (ix == m_btn_num)
    {
        m_selected_page--;
        if (m_selected_page < 0) m_selected_page = 0;
        bindTexturesToButton(m_selected_page);
    }
    else if (ix == m_btn_num + 1)
    {
        m_selected_page++;
        if (m_selected_page > m_tex_list.size() / m_btn_num) m_selected_page-=1;
        bindTexturesToButton(m_selected_page);
    }

} // btnClicked
