#include "InputCache.h";

namespace InputCache {
    bool initialized = false;
    std::unordered_map<int32_t, int32_t> keyStates;
    // parallel arrays are easier to work with. 

    bool initializeCache() {
        if (initialized) return true;

        keyStates.insert({GLFW_KEY_SPACE,        KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_APOSTROPHE,   KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_COMMA,        KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_MINUS,        KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_PERIOD,       KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_SLASH,        KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_0,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_1,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_2,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_3,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_4,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_5,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_6,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_7,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_8,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_9,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_SEMICOLON,    KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_EQUAL,        KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_A,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_B,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_C,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_D,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_E,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_G,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_H,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_I,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_J,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_K,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_L,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_M,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_N,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_O,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_P,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_Q,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_R,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_S,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_T,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_U,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_V,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_W,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_X,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_Y,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_Z,            KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_LEFT_BRACKET, KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_BACKSLASH,    KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_RIGHT_BRACKET,KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_GRAVE_ACCENT, KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_WORLD_1,      KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_WORLD_2,      KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_ESCAPE,       KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_ENTER,        KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_TAB,          KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_BACKSPACE,    KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_INSERT,       KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_DELETE,       KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_RIGHT,        KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_LEFT,         KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_DOWN,         KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_UP,           KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_PAGE_UP,      KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_PAGE_DOWN,    KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_HOME,         KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_END,          KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_CAPS_LOCK,    KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_SCROLL_LOCK,  KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_NUM_LOCK,     KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_PRINT_SCREEN, KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_PAUSE,        KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F1,           KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F2,           KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F3,           KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F4,           KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F5,           KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F6,           KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F7,           KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F8,           KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F9,           KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F10,          KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F11,          KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F12,          KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F13,          KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F14,          KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F15,          KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F16,          KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F17,          KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F18,          KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F19,          KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F20,          KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F21,          KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F22,          KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F23,          KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F24,          KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_F25,          KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_KP_0,         KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_KP_1,         KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_KP_2,         KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_KP_3,         KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_KP_4,         KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_KP_5,         KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_KP_6,         KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_KP_7,         KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_KP_8,         KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_KP_9,         KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_KP_DECIMAL,   KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_KP_DIVIDE,    KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_KP_MULTIPLY,  KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_KP_SUBTRACT,  KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_KP_ADD,       KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_KP_ENTER,     KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_KP_EQUAL,     KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_LEFT_SHIFT,   KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_LEFT_CONTROL, KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_LEFT_ALT,     KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_LEFT_SUPER,   KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_RIGHT_SHIFT,  KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_RIGHT_CONTROL,KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_RIGHT_ALT,    KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_RIGHT_SUPER,  KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_MENU,         KeyState::NEUTRAL});
        keyStates.insert({GLFW_KEY_LAST,         KeyState::NEUTRAL});

        initialized = true;
        return true;
    }

    bool captureKeyAction(int32_t key, int action) {
        if (initialized == false) initializeCache();

        KeyState cachedState;
        if (convertKeyIntToKeyState(keyStates[key], cachedState) == false) 
            return false;
        KeyState actionState;
        if (convertActionIntToKeyState(action, actionState) == false)
            return false;

        
        switch(cachedState)
        {
            case KeyState::NEUTRAL:
                keyStates[key] = actionState;
                break;
            case KeyState::PRESS:
                if (actionState == KeyState::PRESS) { keyStates[key] = KeyState::HOLD; }
                else                                { keyStates[key] = KeyState::RELEASE; }
                break;
            case KeyState::HOLD:
                if (actionState == KeyState::PRESS) {

                }
                break;
            case KeyState::RELEASE:
                stateEnum = KeyState::RELEASE;
                break;
            default:
                stateEnum = KeyState::INVALID;
                // Not a valid key state. 
                return false;
        }

        return true;
    }

    bool convertKeyIntToKeyState(int32_t stateInt, enum KeyState& stateEnum) {
        switch(stateInt) {
            case KeyState::NEUTRAL:
                stateEnum = KeyState::NEUTRAL;
                break;
            case KeyState::PRESS:
                stateEnum = KeyState::PRESS;
                break;
            case KeyState::HOLD:
                stateEnum = KeyState::HOLD;
                break;
            case KeyState::RELEASE:
                stateEnum = KeyState::RELEASE;
                break;
            default:
                stateEnum = KeyState::INVALID;
                // Not a valid key state. 
                return false;
        }

        return true;
    }

    bool convertActionIntToKeyState(int action, enum KeyState& actionState) {
        switch(action) {
            case GLFW_PRESS:
                actionState = KeyState::PRESS;
                break;
            case GLFW_RELEASE:
                actionState = KeyState::RELEASE;
                break;
            default:
                // Not a valid key state. NOT using GLFW_REPEAT. Unreliable. https://www.glfw.org/docs/3.3/input_guide.html
                actionState = KeyState::INVALID;
                return false;
        }
        return true;
    }

}

