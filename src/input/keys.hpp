#ifndef KEYS_HPP
#define KEYS_HPP

#include <irrlicht.h>

using namespace irr;

enum Key
{
    W_PRESSED, // the first has to have id 0
    A_PRESSED,
    S_PRESSED,
    D_PRESSED,
    R_PRESSED,
    G_PRESSED,
    DEL_PRESSED,
    SPACE_PRESSED,
    CTRL_PRESSED,
    SHIFT_PRESSED //it has to be the last the first has to have id 0
};

struct Keys
{
    static const int    m_key_num = SHIFT_PRESSED + 1;
    bool                m_key_state[m_key_num];

    Keys()
    {
        for (int i = 0; i < m_key_num; i++) m_key_state[i] = false;
    }

    bool state(Key k) { return m_key_state[k]; }

    void keyEvent(EKEY_CODE code, bool pressed)
    {
        switch (code)
        {
        case KEY_KEY_W:
            m_key_state[W_PRESSED] = pressed;
            break;
        case KEY_KEY_A:
            m_key_state[A_PRESSED] = pressed;
            break;
        case KEY_KEY_S:
            m_key_state[S_PRESSED] = pressed;
            break;
        case KEY_KEY_D:
            m_key_state[D_PRESSED] = pressed;
            break;
        case KEY_KEY_R:
            m_key_state[R_PRESSED] = pressed;
            break;
        case KEY_KEY_G:
            m_key_state[G_PRESSED] = pressed;
            break;
        case KEY_SPACE:
            m_key_state[SPACE_PRESSED] = pressed;
            break;
        case KEY_DELETE:
            m_key_state[DEL_PRESSED] = pressed;
            break;
        case KEY_SHIFT:
        case KEY_LSHIFT:
            m_key_state[SHIFT_PRESSED] = pressed;
            break;
        case KEY_CONTROL:
        case KEY_LCONTROL:
            m_key_state[CTRL_PRESSED] = pressed;
            break;
        default:
            break;
        }
    } // keyEvent

};

#endif

