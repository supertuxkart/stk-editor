#include "toolbox/envpanel.hpp"

#include "toolbox/library.hpp"
#include "toolbox/element.hpp"
#include "editor.hpp"

#include <list>
#include <assert.h>

EnvPanel* EnvPanel::m_env_panel = 0;

// ----------------------------------------------------------------------------
void EnvPanel::init()
{
    IGUIEnvironment* gui_env = Editor::getEditor()->getGUIEnv();
    dimension2du ss = Editor::getEditor()->getScreenSize();

    m_cb = gui_env->addComboBox(rect<s32>(25, 10, 200, 30), m_tab, CB_ID);
    m_cb->addItem(L"All");
    m_cb->setSelected(0);

    // !before new Lib
    initButtons();

    m_lib = new Library(L"env", m_btn_num);
    std::list<stringw> list = m_lib->getCategoryList();
    std::list<stringw>::iterator it;
    for (it = list.begin(); it != list.end(); it++)
    {
        m_cb->addItem((*it).c_str());
    }
    refreshBtnTable();

    m_search_field = gui_env->addEditBox(L"", rect<s32>(25, 40, 200, 60), true, m_tab);

    m_index = 0;

    m_next = gui_env->addButton(rect<s32>(190, ss.Height - 120, 240, ss.Height - 100),
        m_tab, FIRST_BTN_ID + m_btn_num + 1);

    m_prev = gui_env->addButton(rect<s32>(10, ss.Height - 120, 60, ss.Height - 100),
        m_tab, FIRST_BTN_ID + m_btn_num);

} // init

// ----------------------------------------------------------------------------
void EnvPanel::initButtons()
{
    IGUIEnvironment* gui_env = Editor::getEditor()->getGUIEnv();
    dimension2du ss = Editor::getEditor()->getScreenSize();
    m_btn_num = (ss.Height - 200) / 60 * 4;
    m_btn_table = new std::pair<IGUIButton*, stringw>[m_btn_num];

    for (int i = 0; i < m_btn_num / 4; i++)
    {
        m_btn_table[4 * i].first = gui_env->addButton
            (rect<s32>(10, i * 60 + 80, 60, i * 60 + 130), m_tab, FIRST_BTN_ID + 4 * i);

        m_btn_table[4 * i + 1].first = gui_env->addButton
            (rect<s32>(70, i * 60 + 80, 120, i * 60 + 130), m_tab, FIRST_BTN_ID + 4 * i + 1);

        m_btn_table[4 * i + 2].first = gui_env->addButton
            (rect<s32>(130, i * 60 + 80, 180, i * 60 + 130), m_tab, FIRST_BTN_ID + 4 * i + 2);

        m_btn_table[4 * i + 3].first = gui_env->addButton
            (rect<s32>(190, i * 60 + 80, 240, i * 60 + 130), m_tab, FIRST_BTN_ID + 4 * i + 3);
    }

    for (int i = 0; i < m_btn_num; i++) m_btn_table[i].second = L"";

} // initButtons

// ----------------------------------------------------------------------------
EnvPanel* EnvPanel::getEnvPanel(IGUITab* tab)
{
    if (m_env_panel != 0) return m_env_panel;

    m_env_panel = new EnvPanel();
    m_env_panel->m_tab = tab;
    m_env_panel->init();
    return m_env_panel;
} // getEnvPanel


// ----------------------------------------------------------------------------
stringw EnvPanel::getModelPathFromBtnId(int ID)
{
    assert(ID - FIRST_BTN_ID >= 0 && ID - FIRST_BTN_ID < m_btn_num);
    return m_btn_table[ID - FIRST_BTN_ID].second;
} // getModelPathFromBtnId


// ----------------------------------------------------------------------------
void EnvPanel::refreshBtnTable()
{
    m_lib->selectElements("",m_cb->getItem(m_cb->getSelected()));
    std::list<Element*> elements = m_lib->getElements(m_index);

    std::list<Element*>::iterator it = elements.begin();

    IFileSystem* file_system = Editor::getEditor()->getDevice()->getFileSystem();
    stringw dir = file_system->getWorkingDirectory() + L"/libraries/env/img/";
    int i;
    for (i = 0; i < m_btn_num && it != elements.end(); i++, it++)
    {
        ITexture* img = Editor::loadImg(dir + (*it)->getImg());
        m_btn_table[i].first->setVisible(true);
        m_btn_table[i].first->setImage(img);
        m_btn_table[i].first->setPressedImage(img);
        m_btn_table[i].second = (*it)->getModel();
    }

    for (; i < m_btn_num; i++)
    {
        m_btn_table[i].first->setVisible(false);
    }

} // refreshBtnTable

// ----------------------------------------------------------------------------
void EnvPanel::reallocate(dimension2du ss)
{
    m_next->setRelativePosition(rect<s32>(190, ss.Height - 120, 240, ss.Height - 100));
    m_prev->setRelativePosition(rect<s32>(10, ss.Height - 120, 60, ss.Height - 100));

    int new_btn_num = (ss.Height - 200) / 60 * 4;
    if (new_btn_num != m_btn_num)
    {
        for (int i = 0; i < m_btn_num; i++)
            m_btn_table[i].first->remove();
        delete [] m_btn_table;
        m_lib->setBufferSize(new_btn_num);
        initButtons();
        refreshBtnTable();
        m_next->setID(FIRST_BTN_ID + m_btn_num + 1);
        m_prev->setID(FIRST_BTN_ID + m_btn_num);
    }

} // reallocate

// ----------------------------------------------------------------------------
void EnvPanel::switchPage(int dir)
{
    if (dir < 0 && m_index > 0)
        m_index--;
    else if (dir > 0 && m_index < m_lib->getSelectionPageNum() - 1)
        m_index++;

    refreshBtnTable();

} // switchPage
