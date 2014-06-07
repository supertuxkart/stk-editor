#ifndef TOOLBAR_HPP
#define TOOLBAR_HPP

#include <irrlicht.h>

using namespace irr;
using namespace video;
using namespace gui;

#include <string>

class ToolBar
{
public:

    // toolbar button id-s:
    enum ToolboxButtonIndex
    {
        TBI_NEW,
        TBI_OPEN,
        TBI_SAVE,
        TBI_SAVE_AS,
        TBI_REDO,
        TBI_UNDO,
        TBI_CAM,
        TBI_SELECT,
        TBI_MOVE,
        TBI_ROTATE,
        TBI_SCALE,
        TBI_DELETE,
        TBI_GRID_ON_OFF,
        TBI_GRID_INC,
        TBI_GRID_DEC,
        TBI_TRY,
        TBI_SETTINGS,
        TBI_EXIT
    };

private:
    // private variables:

    static ToolBar* m_toolbar;

    IGUIToolBar*    m_bar;
    IGUIButton*     m_buttons[18];

    // private functions:

    void            init();
    ITexture*       loadImg(const char* file_path);
    void            buttonInit(unsigned int ix, ToolboxButtonIndex TBI, 
                    const char* file_path, const wchar_t* text);
    ToolBar() {};

public:
    static ToolBar*     getToolBar();
    void                reallocate();
};

#endif
