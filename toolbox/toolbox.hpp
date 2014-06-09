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

    IGUITab*        m_env_tab;
    unsigned int    m_env_index;
    IGUIButton*     m_env_next;
    IGUIButton*     m_env_prev;
    int             m_env_btn_num;

    std::pair<IGUIButton*, stringw> *m_env_btn_table;

    // private functions:

    void    init();
    void    initEnvTab();
    void    initEnvTabButtons();

    ToolBox() {};

public:
    static ToolBox*     getToolBox();
    stringw             getEnvModelPathFromBtnId(int ID);
    void                refreshEnvBtnTable();
    void                reallocate();

    void                switchEnvPage(int dir);
    int                 getEnvBtnNum()              { return m_env_btn_num; }
    void                resetEnvIndex()             { m_env_index = 0;      }
};

#endif
