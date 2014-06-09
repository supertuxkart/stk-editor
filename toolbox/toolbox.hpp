#ifndef TOOLBOX_HPP
#define TOOLBOX_HPP

#include <irrlicht.h>

#include<map>

class Library;

using namespace irr;
using namespace gui;
using namespace core;

class ToolBox
{
public:
    static const int ENV_BTN_ID  = 100;
    static const int ENV_BTN_NUM = 32;
    static const int ENV_CB_ID   = 1111;
    static const int TBOX_ID     = 1112;
private:

    // private variables:

    static ToolBox* m_toolbox;

    IGUIWindow*     m_boxwnd;
    IGUITabControl* m_tab;

    Library*        m_env_lib;
    IGUIComboBox*   m_env_cb;
    IGUIEditBox*    m_env_search_field;

    unsigned int    m_env_index;
    IGUIButton*     m_env_next;
    IGUIButton*     m_env_prev;

    std::pair<IGUIButton*, stringw> m_env_btn_table[ENV_BTN_NUM];

    // private functions:

    void    init();
    void    initEnvTab();

    ToolBox() {};

public:
    static ToolBox*     getToolBox();
    stringw             getEnvModelPathFromBtnId(int ID);
    void                refreshEnvBtnTable();
    void                reallocate();

    void                switchEnvPage(int dir);
};

#endif
