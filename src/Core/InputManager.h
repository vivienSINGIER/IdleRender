#ifndef INPUT_SYSTEM_H_INCLUDED
#define INPUT_SYSTEM_H_INCLUDED

#include "define.h"
#include "Math/Vector/Vector.h"

enum InputKeyboard 
{
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    _0, _1, _2, _3, _4, _5, _6, _7, _8, _9,
    NUMPAD0, NUMPAD1, NUMPAD2, NUMPAD3, NUMPAD4, NUMPAD5, NUMPAD6, NUMPAD7, NUMPAD8, NUMPAD9,
    NUMPAD_DIVIDE, NUMPAD_MULTIPLY, NUMPAD_SUBTRACT, NUMPAD_ADD, NUMPAD_RETURN, NUMPAD_DECIMAL,

    NUMLOCK, CAPSLOCK, SCROLL_LOCK,
    PAUSE,

    ²,

    LCTRL, RCTRL,
    LSHIFT, RSHIFT,
    LALT, RALT,
    LWINDOW, RWINDOW,

    UP, LEFT, DOWN, RIGHT,

    ESCAPE, TAB, SPACE, BACKSPACE, RETURN,
    INSERT, DELETE_,
    HOME, END,
    PAGE_UP, PAGE_DOWN,

    AMOUNT_KEY,

    ESC = ESCAPE,
    SPACEBAR = SPACE,

    ENTER = RETURN,
    NUMPAD_ENTER = NUMPAD_RETURN,

    LCONTROL = LCTRL,
    RCONTROL = RCTRL,
    ALTGR = RALT,

    UP_ARROW = UP,
    LEFT_ARROW = LEFT,
    DOWN_ARROW = DOWN,
    RIGHT_ARROW = RIGHT,
};

enum InputMouse 
{
    LEFT_MOUSE,
    RIGHT_MOUSE,
    MIDDLE_MOUSE,
    AMOUNT_MOUSE
};

enum InputState
{
    DOWN_STATE,
    PRESSED_STATE,
    UP_STATE,
    RELEASED_STATE,
};

struct MouseData
{
    bool cursorLocked = false;
    bool cursorVisible = true;
    int cursorVisibilityCount = 0;
    
    int32 x, y;
    float deltaX, deltaY;
    
    bool dirty = false;
};

class InputManager
{
public:
    InputManager() = default;
    ~InputManager() = default;

    static void Initialize(HWND hwnd);

    static void HandleInput();
    
    static void SetKeyState(InputKeyboard _key, InputState _state);
    
    static bool IsKeyDown(InputKeyboard _key);
    static bool IsKey(InputKeyboard _key);
    static bool IsKeyUp(InputKeyboard _key);
    
    static bool IsMouseButtonPressed(InputMouse _key);
    static bool IsMouseButtonUp(InputMouse _key);
    static bool IsMouseButtonDown(InputMouse _key);
    
    static void SetMouseButtonState(InputMouse _key, InputState _state);
    
    static Vect2i32 GetMousePosition();
    static Vect2i32 GetCenteredMousePosition(Vect2i32 const& _winSize);
    static Vect2f32 GetMouseDelta();
    static void SetMousePosition(Vect2i32 const& coordinates);
    
    static void LockMouseCursor();
    static void UnlockMouseCursor();
    static bool IsMouseCursorLocked();
    
    static void ShowMouseCursor();
    static void HideMouseCursor();
    static bool IsMouseCursorVisible();

    static bool IsCursorLocked();

private:
    static UnorderedMap<UINT8, INT32> s_keyboardMap;
    static UnorderedMap<UINT8, INT32> s_mouseMap;
    
    inline static Array<InputState, AMOUNT_KEY> s_keyboardState;
    inline static Array<InputState, AMOUNT_KEY>  s_lastKeyboardState;
    
    inline static Array<InputState, AMOUNT_MOUSE> s_mouseState;
    inline static Array<InputState, AMOUNT_MOUSE> s_lastMouseState;
    inline static MouseData s_mouseData;
    
    inline static HWND s_pHWND = nullptr;

    static bool m_locked;
};


#endif
