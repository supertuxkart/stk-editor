#include "toolbox/toolbox.hpp"

#include "toolbox/library.hpp"
#include "toolbox/element.hpp"
#include "editor.hpp"

#include <list>
#include <assert.h>

ToolBox* ToolBox::m_toolbox = 0;

// ----------------------------------------------------------------------------
void ToolBox::init()
{
    IGUIEnvironment* gui_env = Editor::getEditor()->getGUIEnv();
    dimension2du ss = Editor::getEditor()->getScreenSize();

    m_boxwnd = gui_env->addWindow(
            rect<s32>(),
            false, L"Stuffs", 0, TBOX_ID
        );
    m_boxwnd->setMinSize(dimension2du(250, ss.Height-50));
    m_boxwnd->setRelativePosition(position2di(ss.Width-250,50));
    m_boxwnd->setDraggable(false);
    m_boxwnd->getCloseButton()->setVisible(false);
    m_boxwnd->setDrawTitlebar(false);

    // create tab control and tabs
    m_tab = gui_env->addTabControl(
        core::rect<s32>(), m_boxwnd, true, true);

    m_tab->setMinSize(dimension2du(250, ss.Height - 50));
    m_tab->setRelativePosition(position2di(0, 0));

    m_tab->addTab(L"Terrain");
    m_tab->addTab(L"Road");

    initEnvTab();

    m_tab->addTab(L"Extra");
    m_tab->addTab(L"Blocks");

} // init

// ----------------------------------------------------------------------------
void ToolBox::initEnvTab()
{
    IGUIEnvironment* gui_env = Editor::getEditor()->getGUIEnv();

    IGUITab* env = m_tab->addTab(L"Env");

    m_env_cb = gui_env->addComboBox(rect<s32>(25, 10, 200, 30), env, ENV_CB_ID);
    m_env_cb->addItem(L"All");
    m_env_cb->setSelected(0);
    m_env_lib = new Library(L"env",ENV_BTN_NUM);

    std::list<stringw> list = m_env_lib->getCategoryList();
    std::list<stringw>::iterator it;

    for (it = list.begin(); it != list.end(); it++)
    {
        m_env_cb->addItem((*it).c_str());
    }

    m_env_search_field = gui_env->addEditBox(L"", rect<s32>(25, 40, 200, 60), true, env);

    for (int i = 0; i < 10; i++)
    {
        m_env_btn_table[4 * i].first = gui_env->addButton
            (rect<s32>(10, i * 60 + 80, 60, i * 60 + 130), env, ENV_BTN_ID + 4 * i);

        m_env_btn_table[4 * i + 1].first = gui_env->addButton
            (rect<s32>(70, i * 60 + 80, 120, i * 60 + 130), env, ENV_BTN_ID + 4 * i + 1);

        m_env_btn_table[4 * i + 2].first = gui_env->addButton
            (rect<s32>(130, i * 60 + 80, 180, i * 60 + 130), env, ENV_BTN_ID + 4 * i + 2);

        m_env_btn_table[4 * i + 3].first = gui_env->addButton
            (rect<s32>(190, i * 60 + 80, 240, i * 60 + 130), env, ENV_BTN_ID + 4 * i + 3);
    }

    for (int i = 0; i < ENV_BTN_NUM; i++) m_env_btn_table[i].second = L"";

    refreshEnvBtnTable();
}

// ----------------------------------------------------------------------------
ToolBox* ToolBox::getToolBox()
{
    if (m_toolbox != 0) return m_toolbox;

    m_toolbox = new ToolBox();
    m_toolbox->init();
    return m_toolbox;
} // getToolBox


// ----------------------------------------------------------------------------
stringw ToolBox::getEnvModelPathFromBtnId(int ID)
{
    assert(ID - ENV_BTN_ID >= 0 && ID - ENV_BTN_ID < ENV_BTN_NUM);
    return m_env_btn_table[ID - ENV_BTN_ID].second;
} // getEnvModelPathFromBtnId


// ----------------------------------------------------------------------------
void ToolBox::refreshEnvBtnTable()
{
    m_env_lib->selectElements("",m_env_cb->getItem(m_env_cb->getSelected()));
    std::list<Element*> elements = m_env_lib->getElements();

    std::list<Element*>::iterator it = elements.begin();

    IFileSystem* file_system = Editor::getEditor()->getDevice()->getFileSystem();
    stringw dir = file_system->getWorkingDirectory() + L"/libraries/env/img/";
    int i;
    for (i = 0; i < ENV_BTN_NUM && it != elements.end(); i++, it++)
    {
        m_env_btn_table[i].first->setVisible(true);
        m_env_btn_table[i].first->setImage(Editor::loadImg(dir + (*it)->getImg()));
        m_env_btn_table[i].second = (*it)->getModel();
    }

    for (; i < ENV_BTN_NUM; i++)
    {
        m_env_btn_table[i].first->setVisible(false);
    }

} // refreshEnvBtnTable

// ----------------------------------------------------------------------------
void ToolBox::reallocate()
{
    dimension2du ss = Editor::getEditor()->getScreenSize();

    m_boxwnd->setMinSize(dimension2du(250, ss.Height-50));
    m_boxwnd->setRelativePosition(position2di(ss.Width-250,50));
    m_tab->setMinSize(dimension2du(250, ss.Height - 50));

} // reallocate
