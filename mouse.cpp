#include "mouse.hpp"


MouseData::MouseData()
{
    in_view = false;

    wheel = 0;
    left_btn_down = false;
    right_btn_down = false;

    left_pressed = false;
    right_pressed = false;

    left_released = false;
    right_released = false;

    x = 0;
    y = 0;
    prev_x = 0;
    prev_y = 0;
} // MouseData

void MouseData::refresh(const SEvent& e)
{
    switch (e.MouseInput.Event)
    {
    case EMIE_MOUSE_WHEEL:
        wheel += e.MouseInput.Wheel;
        break;
    case EMIE_LMOUSE_PRESSED_DOWN:
        left_btn_down = true;
        left_pressed = true;
        left_released = false;
        break;
    case EMIE_LMOUSE_LEFT_UP:
        left_btn_down = false;
        left_pressed = false;
        left_released = true;
        break;
    case EMIE_RMOUSE_PRESSED_DOWN:
        right_btn_down = true;
        right_pressed = true;
        right_released = false;
        break;
    case EMIE_RMOUSE_LEFT_UP:
        right_btn_down = false;
        right_pressed = false;
        right_released = true;
        break;
    case EMIE_MOUSE_MOVED:
        x = e.MouseInput.X;
        y = e.MouseInput.Y;
        break;
    default:
        break;
    }
} // refresh

