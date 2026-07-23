#ifndef INPUT_SYSTEM_CPP_INCLUDED
#define INPUT_SYSTEM_CPP_INCLUDED

#include "InputManager.h"
#include "Math/Vector/Vector.h"

bool InputManager::m_locked = false;

UnorderedMap<UINT8, INT32> InputManager::s_keyboardMap{
    { BACKSPACE,       VK_BACK     },
    { TAB,             VK_TAB      },
    { RETURN,          VK_RETURN   },
    { PAUSE,           VK_PAUSE    },
    { CAPSLOCK,        VK_CAPITAL  },
    { ESCAPE,          VK_ESCAPE   },
    { SPACE,           VK_SPACE    },
    { PAGE_UP,         VK_PRIOR    },
    { PAGE_DOWN,       VK_NEXT     },
    { END,             VK_END      },
    { HOME,            VK_HOME     },
    { LEFT,            VK_LEFT     },
    { UP,              VK_UP       },
    { RIGHT,           VK_RIGHT    },
    { DOWN,            VK_DOWN     },
    { INSERT,          VK_INSERT   },
    { DELETE_,         VK_DELETE   },
    { LWINDOW,         VK_LWIN     },
    { RWINDOW,         VK_RWIN     },
    { NUMPAD0,         VK_NUMPAD0  },
    { NUMPAD1,         VK_NUMPAD1  },
    { NUMPAD2,         VK_NUMPAD2  },
    { NUMPAD3,         VK_NUMPAD3  },
    { NUMPAD4,         VK_NUMPAD4  },
    { NUMPAD5,         VK_NUMPAD5  },
    { NUMPAD6,         VK_NUMPAD6  },
    { NUMPAD7,         VK_NUMPAD7  },
    { NUMPAD8,         VK_NUMPAD8  },
    { NUMPAD9,         VK_NUMPAD9  },
    { NUMPAD_MULTIPLY, VK_MULTIPLY },
    { NUMPAD_ADD,      VK_ADD      },
    { NUMPAD_SUBTRACT, VK_SUBTRACT },
    { NUMPAD_DECIMAL,  VK_DECIMAL  },
    { NUMPAD_DIVIDE,   VK_DIVIDE   },
    { F1,              VK_F1       },
    { F2,              VK_F2       },
    { F3,              VK_F3       },
    { F4,              VK_F4       },
    { F5,              VK_F5       },
    { F6,              VK_F6       },
    { F7,              VK_F7       },
    { F8,              VK_F8       },
    { F9,              VK_F9       },
    { F10,             VK_F10      },
    { F11,             VK_F11      },
    { F12,             VK_F12      },
    { NUMLOCK,         VK_NUMLOCK  },
    { SCROLL_LOCK,     VK_SCROLL   },
    { LSHIFT,          VK_LSHIFT   },
    { RSHIFT,          VK_RSHIFT   },
    { LCONTROL,        VK_LCONTROL },
    { RCONTROL,        VK_RCONTROL },
    { LALT,            VK_LMENU    },
    { RALT,            VK_RMENU    },
    { A,              'A'          },
    { B,              'B'          },
    { C,              'C'          },
    { D,              'D'          },
    { E,              'E'          },
    { F,              'F'          },
    { G,              'G'          },
    { H,              'H'          },
    { I,              'I'          },
    { J,              'J'          },
    { K,              'K'          },
    { L,              'L'          },
    { M,              'M'          },
    { N,              'N'          },
    { O,              'O'          },
    { P,              'P'          },
    { Q,              'Q'          },
    { R,              'R'          },
    { S,              'S'          },
    { T,              'T'          },
    { U,              'U'          },
    { V,              'V'          },
    { W,              'W'          },
    { X,              'X'          },
    { Y,              'Y'          },
    { Z,              'Z'          },
    { _0,             '0'          },
    { _1,             '1'          },
    { _2,             '2'          },
    { _3,             '3'          },
    { _4,             '4'          },
    { _5,             '5'          },
    { _6,             '6'          },
    { _7,             '7'          },
    { _8,             '8'          },
    { _9,             '9'          },
    { ²,              VK_OEM_7     },
};

UnorderedMap<UINT8, INT32> InputManager::s_mouseMap{
    { LEFT_MOUSE,   VK_LBUTTON  },
    { RIGHT_MOUSE,  VK_RBUTTON  },
    { MIDDLE_MOUSE, VK_MBUTTON  }
};


void InputManager::Initialize(HWND pHWND)
{
	s_pHWND = pHWND;
}

void InputManager::HandleInput()
{
    Array<InputState, AMOUNT_KEY>& kb = s_keyboardState;
    Array<InputState, AMOUNT_MOUSE>& mouse = s_mouseState;
    MouseData& mData = s_mouseData;
    
    if (GetForegroundWindow() != s_pHWND)
        return;
    
    for (const Pair<unsigned char, int> input : s_keyboardMap)
    {
        unsigned char inputKey = input.first;
        int indexKey = input.second;

        bool isKeyDown = (GetAsyncKeyState(indexKey) & 0x8000) != 0;
        InputState currentState = kb[inputKey];

        if (isKeyDown)
        {
            if (currentState == DOWN_STATE || currentState == PRESSED_STATE)
                kb[inputKey] = PRESSED_STATE;
            else
                kb[inputKey] = DOWN_STATE;
        }
        else
        {
            if (currentState == DOWN_STATE || currentState == PRESSED_STATE)
                kb[inputKey] = UP_STATE;
            else
                kb[inputKey] = RELEASED_STATE;
        }
    }

    for (const Pair<unsigned char, int> input : s_mouseMap)
    {
        unsigned char inputButton = input.first;
        int indexButton = input.second;

        bool isButtonDown = (GetAsyncKeyState(indexButton) & 0x8000) != 0;
        InputState currentState = mouse[inputButton];

        if (isButtonDown)
        {
            if (currentState == DOWN_STATE || currentState == PRESSED_STATE)
                mouse[inputButton] = PRESSED_STATE;
            else
                mouse[inputButton] = DOWN_STATE;
        }
        else
        {
            if (currentState == DOWN_STATE || currentState == PRESSED_STATE)
                mouse[inputButton] = UP_STATE;
            else
                mouse[inputButton] = RELEASED_STATE;
        }
    }

    POINT p;
    GetCursorPos(&p);
    ScreenToClient(s_pHWND, &p);

    Vect2i32 newPos = { p.x, p.y };

    if (mData.cursorLocked)
    {
        RECT rect;
        GetClientRect(s_pHWND, &rect);

        int centerX = (rect.right - rect.left) / 2;
        int centerY = (rect.bottom - rect.top) / 2;

        mData.deltaX = newPos.x - centerX;
        mData.deltaY = newPos.y - centerY;

        POINT center = { centerX, centerY };
        ClientToScreen(s_pHWND, &center);
        SetCursorPos(center.x, center.y);

        mData.x = centerX;
        mData.y = centerY;
    }
    else
    {
        mData.deltaX = newPos.x - mData.x;
        mData.deltaY = newPos.y - mData.y;

        mData.x = newPos.x;
        mData.y = newPos.y;
    }

    mData.dirty = true;
}

void InputManager::SetKeyState(InputKeyboard _key, InputState _state)
{
    s_keyboardState[_key] = _state;
}

bool InputManager::IsKey(InputKeyboard key)
{
    return s_keyboardState[key] == PRESSED_STATE;
}

bool InputManager::IsKeyUp(InputKeyboard key)
{
    return s_keyboardState[key] == UP_STATE;
}

bool InputManager::IsKeyDown(InputKeyboard key)
{
    return s_keyboardState[key] == DOWN_STATE;
}

bool InputManager::IsMouseButtonPressed(InputMouse key)
{
    return s_mouseState[key] == PRESSED_STATE;
}

bool InputManager::IsMouseButtonUp(InputMouse key)
{
    return s_mouseState[key] == UP_STATE;
}

bool InputManager::IsMouseButtonDown(InputMouse key)
{
    return s_mouseState[key] == DOWN_STATE;
}

void InputManager::SetMouseButtonState(InputMouse _key, InputState _state)
{
    s_mouseState[_key] = _state;
}

Vect2i32 InputManager::GetMousePosition()
{
    return { s_mouseData.x, s_mouseData.y };
}

Vect2i32 InputManager::GetCenteredMousePosition(Vect2i32 const& _winSize)
{
    Vect2i32 mousePos = GetMousePosition();

    int width = _winSize.x;
    int height = _winSize.y;

    mousePos.x -= width / 2;
    mousePos.y -= height / 2;

    return mousePos;
}

Vect2f32 InputManager::GetMouseDelta()
{
    return { s_mouseData.deltaX, s_mouseData.deltaY };
}

void InputManager::SetMousePosition( Vect2i32 const& coordinates)
{
    POINT p{ coordinates.x, coordinates.y };
    ClientToScreen( s_pHWND, &p );
    SetCursorPos( p.x, p.y );
    
    s_mouseData.x = coordinates.x;
    s_mouseData.y = coordinates.y;
    s_mouseData.dirty = true;
}

void InputManager::LockMouseCursor()
{
    if ( s_pHWND == nullptr ) return;
    s_mouseData.cursorLocked = true;
    
    RECT clientRect;
    if ( GetClientRect( s_pHWND, &clientRect ) == false ) return;

    POINT topLeft = { clientRect.left, clientRect.top };
    POINT bottomRight = { clientRect.right, clientRect.bottom };

    ClientToScreen( s_pHWND, &topLeft );
    ClientToScreen( s_pHWND, &bottomRight );

    RECT const clipRect = { topLeft.x, topLeft.y, bottomRight.x, bottomRight.y };
    ClipCursor( &clipRect );
    s_mouseData.dirty = true;
   
	m_locked = true;
}

void InputManager::UnlockMouseCursor()
{
    s_mouseData.cursorLocked = false;
    ClipCursor( nullptr );
    s_mouseData.dirty = true;

	m_locked = false;
}

bool InputManager::IsMouseCursorLocked()
{
    return s_mouseData.cursorLocked;
}

void InputManager::ShowMouseCursor()
{
    if (  s_mouseData.cursorVisible ) return;
    s_mouseData.cursorVisible = true;

    while ( s_mouseData.cursorVisibilityCount < 0 )
        s_mouseData.cursorVisibilityCount = ShowCursor( TRUE );
    s_mouseData.dirty = true;
}

void InputManager::HideMouseCursor()
{
    
    if ( !s_mouseData.cursorVisible ) return;
    s_mouseData.cursorVisible = false;

    while ( s_mouseData.cursorVisibilityCount >= 0 )
        s_mouseData.cursorVisibilityCount = ShowCursor( FALSE );
    s_mouseData.dirty = true;
}

bool InputManager::IsMouseCursorVisible()
{
    return s_mouseData.cursorVisible;
}

bool InputManager::IsCursorLocked()
{
    return m_locked;
}

#endif
