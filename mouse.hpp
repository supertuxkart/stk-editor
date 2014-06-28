#ifndef MOUSE_HPP
#define MOUSE_HPP

#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace core;
using namespace video;



struct MouseData
{
    bool                    in_view;
    f32                     wheel;
    bool                    left_btn_down;
    bool                    right_btn_down;

    bool                    left_pressed;
    bool                    right_pressed;

    bool                    left_released;
    bool                    right_released;

    s32                     x;
    s32                     y;
    s32                     prev_x;
    s32                     prev_y;

// ----------------------------------------------------------------------------
    MouseData();
    void refresh(const SEvent& e);


    void setStorePoint()    { prev_x = x; prev_y = y; }
    s32 dx()                { return x - prev_x; }
    s32 dy()                { return y - prev_y; }

    // return value is true if left mouse button was pressed down after the last call
    bool leftPressed()        { bool b = left_pressed;    left_pressed = false; return b; }

    // return value is true if right mouse button was pressed down after the last call
    bool rightPressed()       { bool b = right_pressed;   right_pressed = false; return b; }

    // return value is true if left mouse button was released after the last call
    bool leftReleased()       { bool b = left_released;  left_released = false; return b; }

    // return value is true if right mouse button was released after the last call
    bool rightReleased()      { bool b = right_released; right_released = false; return b; }

};


#endif
