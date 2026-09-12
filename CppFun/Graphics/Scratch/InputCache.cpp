#include "InputCache.h"

namespace InputCache {
    bool initialized = false;
    std::vector<int32_t> keys;
    std::vector<enum KeyState> states;
    // parallel arrays are easier to work with. 

#pragma region INITIALIZATION

    bool initializeCache() {
        if (initialized) return true;

        keys.reserve(130);
        states.reserve(130);

        keys.emplace_back(GLFW_KEY_SPACE        );
        keys.emplace_back(GLFW_KEY_APOSTROPHE   );
        keys.emplace_back(GLFW_KEY_COMMA        );
        keys.emplace_back(GLFW_KEY_MINUS        );
        keys.emplace_back(GLFW_KEY_PERIOD       );
        keys.emplace_back(GLFW_KEY_SLASH        );
        keys.emplace_back(GLFW_KEY_0            );
        keys.emplace_back(GLFW_KEY_1            );
        keys.emplace_back(GLFW_KEY_2            );
        keys.emplace_back(GLFW_KEY_3            );
        keys.emplace_back(GLFW_KEY_4            );
        keys.emplace_back(GLFW_KEY_5            );
        keys.emplace_back(GLFW_KEY_6            );
        keys.emplace_back(GLFW_KEY_7            );
        keys.emplace_back(GLFW_KEY_8            );
        keys.emplace_back(GLFW_KEY_9            );
        keys.emplace_back(GLFW_KEY_SEMICOLON    );
        keys.emplace_back(GLFW_KEY_EQUAL        );
        keys.emplace_back(GLFW_KEY_A            );
        keys.emplace_back(GLFW_KEY_B            );
        keys.emplace_back(GLFW_KEY_C            );
        keys.emplace_back(GLFW_KEY_D            );
        keys.emplace_back(GLFW_KEY_E            );
        keys.emplace_back(GLFW_KEY_F            );
        keys.emplace_back(GLFW_KEY_G            );
        keys.emplace_back(GLFW_KEY_H            );
        keys.emplace_back(GLFW_KEY_I            );
        keys.emplace_back(GLFW_KEY_J            );
        keys.emplace_back(GLFW_KEY_K            );
        keys.emplace_back(GLFW_KEY_L            );
        keys.emplace_back(GLFW_KEY_M            );
        keys.emplace_back(GLFW_KEY_N            );
        keys.emplace_back(GLFW_KEY_O            );
        keys.emplace_back(GLFW_KEY_P            );
        keys.emplace_back(GLFW_KEY_Q            );
        keys.emplace_back(GLFW_KEY_R            );
        keys.emplace_back(GLFW_KEY_S            );
        keys.emplace_back(GLFW_KEY_T            );
        keys.emplace_back(GLFW_KEY_U            );
        keys.emplace_back(GLFW_KEY_V            );
        keys.emplace_back(GLFW_KEY_W            );
        keys.emplace_back(GLFW_KEY_X            );
        keys.emplace_back(GLFW_KEY_Y            );
        keys.emplace_back(GLFW_KEY_Z            );
        keys.emplace_back(GLFW_KEY_LEFT_BRACKET );
        keys.emplace_back(GLFW_KEY_BACKSLASH    );
        keys.emplace_back(GLFW_KEY_RIGHT_BRACKET);
        keys.emplace_back(GLFW_KEY_GRAVE_ACCENT );
        keys.emplace_back(GLFW_KEY_WORLD_1      );
        keys.emplace_back(GLFW_KEY_WORLD_2      );
        keys.emplace_back(GLFW_KEY_ESCAPE       );
        keys.emplace_back(GLFW_KEY_ENTER        );
        keys.emplace_back(GLFW_KEY_TAB          );
        keys.emplace_back(GLFW_KEY_BACKSPACE    );
        keys.emplace_back(GLFW_KEY_INSERT       );
        keys.emplace_back(GLFW_KEY_DELETE       );
        keys.emplace_back(GLFW_KEY_RIGHT        );
        keys.emplace_back(GLFW_KEY_LEFT         );
        keys.emplace_back(GLFW_KEY_DOWN         );
        keys.emplace_back(GLFW_KEY_UP           );
        keys.emplace_back(GLFW_KEY_PAGE_UP      );
        keys.emplace_back(GLFW_KEY_PAGE_DOWN    );
        keys.emplace_back(GLFW_KEY_HOME         );
        keys.emplace_back(GLFW_KEY_END          );
        keys.emplace_back(GLFW_KEY_CAPS_LOCK    );
        keys.emplace_back(GLFW_KEY_SCROLL_LOCK  );
        keys.emplace_back(GLFW_KEY_NUM_LOCK     );
        keys.emplace_back(GLFW_KEY_PRINT_SCREEN );
        keys.emplace_back(GLFW_KEY_PAUSE        );
        keys.emplace_back(GLFW_KEY_F1           );
        keys.emplace_back(GLFW_KEY_F2           );
        keys.emplace_back(GLFW_KEY_F3           );
        keys.emplace_back(GLFW_KEY_F4           );
        keys.emplace_back(GLFW_KEY_F5           );
        keys.emplace_back(GLFW_KEY_F6           );
        keys.emplace_back(GLFW_KEY_F7           );
        keys.emplace_back(GLFW_KEY_F8           );
        keys.emplace_back(GLFW_KEY_F9           );
        keys.emplace_back(GLFW_KEY_F10          );
        keys.emplace_back(GLFW_KEY_F11          );
        keys.emplace_back(GLFW_KEY_F12          );
        keys.emplace_back(GLFW_KEY_F13          );
        keys.emplace_back(GLFW_KEY_F14          );
        keys.emplace_back(GLFW_KEY_F15          );
        keys.emplace_back(GLFW_KEY_F16          );
        keys.emplace_back(GLFW_KEY_F17          );
        keys.emplace_back(GLFW_KEY_F18          );
        keys.emplace_back(GLFW_KEY_F19          );
        keys.emplace_back(GLFW_KEY_F20          );
        keys.emplace_back(GLFW_KEY_F21          );
        keys.emplace_back(GLFW_KEY_F22          );
        keys.emplace_back(GLFW_KEY_F23          );
        keys.emplace_back(GLFW_KEY_F24          );
        keys.emplace_back(GLFW_KEY_F25          );
        keys.emplace_back(GLFW_KEY_KP_0         );
        keys.emplace_back(GLFW_KEY_KP_1         );
        keys.emplace_back(GLFW_KEY_KP_2         );
        keys.emplace_back(GLFW_KEY_KP_3         );
        keys.emplace_back(GLFW_KEY_KP_4         );
        keys.emplace_back(GLFW_KEY_KP_5         );
        keys.emplace_back(GLFW_KEY_KP_6         );
        keys.emplace_back(GLFW_KEY_KP_7         );
        keys.emplace_back(GLFW_KEY_KP_8         );
        keys.emplace_back(GLFW_KEY_KP_9         );
        keys.emplace_back(GLFW_KEY_KP_DECIMAL   );
        keys.emplace_back(GLFW_KEY_KP_DIVIDE    );
        keys.emplace_back(GLFW_KEY_KP_MULTIPLY  );
        keys.emplace_back(GLFW_KEY_KP_SUBTRACT  );
        keys.emplace_back(GLFW_KEY_KP_ADD       );
        keys.emplace_back(GLFW_KEY_KP_ENTER     );
        keys.emplace_back(GLFW_KEY_KP_EQUAL     );
        keys.emplace_back(GLFW_KEY_LEFT_SHIFT   );
        keys.emplace_back(GLFW_KEY_LEFT_CONTROL );
        keys.emplace_back(GLFW_KEY_LEFT_ALT     );
        keys.emplace_back(GLFW_KEY_LEFT_SUPER   );
        keys.emplace_back(GLFW_KEY_RIGHT_SHIFT  );
        keys.emplace_back(GLFW_KEY_RIGHT_CONTROL);
        keys.emplace_back(GLFW_KEY_RIGHT_ALT    );
        keys.emplace_back(GLFW_KEY_RIGHT_SUPER  );
        keys.emplace_back(GLFW_KEY_MENU         );
        keys.emplace_back(GLFW_KEY_LAST         );



        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);
        states.emplace_back(KeyState::NEUTRAL);

        keys.shrink_to_fit();
        states.shrink_to_fit();

        initialized = true;
        return true;
    }

#pragma endregion =====================================================================================================================

#pragma region STATE_CHANGING_FUNCTIONS

    bool cacheKeyState(int32_t key, int action) {
        if (isCacheInitialized() == false) return false;

        int32_t keyIndex;
        if (findKeyCodeIndex(key, keyIndex) == false) return false;

        enum KeyState cachedState;
        if (convertStateIntToStateEnum(states[keyIndex], cachedState) == false) return false;

        enum KeyState actionState;
        if (convertActionIntToKeyState(action, actionState) == false) return false;

        if (actionState == KeyState::INVALID)
        {
            // std::cerr << "NOT using GLFW_REPEAT. Unreliable. See: https://www.glfw.org/docs/3.3/input_guide.html" << std::endl;
            return false;
        }
        
        switch(cachedState)
        {
            case KeyState::NEUTRAL:
                states[keyIndex] = actionState; // new state *should* be press. Make a check??
                break;
            case KeyState::PRESS:
                if (actionState == KeyState::PRESS) { states[keyIndex] = KeyState::HOLD; }
                else                                { states[keyIndex] = KeyState::RELEASE; }
                break;
            case KeyState::HOLD:
                if (actionState == KeyState::PRESS) { /* Do nothing */ }
                else if (actionState == KeyState::RELEASE) {states[keyIndex] = KeyState::RELEASE; }
                break;
            case KeyState::RELEASE:
                if (actionState == KeyState::PRESS) { states[keyIndex] = KeyState::PRESS; }
                else                                { states[keyIndex] = KeyState::NEUTRAL; }
            default:
                // Cached state is INVALID. Should NEVER happen.
                std::cerr << "INVALID CACHED STATE: " << std::to_string(cachedState) << " Action State: " << std::to_string(actionState) << std::endl;
                return false;
        }

        return true;
    }

    //-/////////////////////////////////////////////////////////
    // updatePressedAndReleased() - updates one-frame state to persistent state
    // Pressed --> Hold
    // Released --> Neutral
    //
    // Call this after processing input in a frame so state is updated for next frame.
    bool updateSingleFrameStates() {
        for (unsigned int i = 0; i < states.size(); ++i)
        {
            // state is validated in other functions. Not checking all states here.
            switch(states[i])
            {
                case KeyState::PRESS: { states[i] = KeyState::HOLD; break;}
                case KeyState::RELEASE: { states[i] = KeyState::NEUTRAL; break;}
                default: break;
            }
        }

        return true;
    }

#pragma endregion =====================================================================================================================

#pragma region STATE_ACCESSING_FUNCTIONS

    bool getState(int32_t glfwKeyCode, enum KeyState& outState) {
        if (isCacheInitialized() == false) return false;

        int32_t index;
        if (findKeyCodeIndex(glfwKeyCode, index)== false ) return false;

        if (isValidStateValue(states[index]) == false) return false;

        outState = states[index];
        
        return true;
    }

    bool isKeyState(int32_t glfwKeyCode, enum InputCache::KeyState stateValue) {
        if (isCacheInitialized() == false) return false;
        enum InputCache::KeyState outState;
        InputCache::getState(glfwKeyCode, outState);
        if (outState == stateValue) return true;
        else                        return false;
    }

    bool findKeyCodeIndex(int32_t keyInt, int32_t& keyIndex) {
        if (isCacheInitialized() == false) return false;

        for (unsigned int i = 0; i < keys.size(); ++i) {
            if (keyInt == keys[i]) {
                keyIndex = i;
                return true;
            }
        }

        // Could not find keycode
        std::cerr << "FAILED to find key matching value: " << std::to_string(keyInt);
        return false;
    }

#pragma endregion =====================================================================================================================


#pragma region HELPER_FUNCTIONS

    bool isCacheInitialized(){
        if (initializeCache() == false)
        {
            std::cerr << "FAILED to initialize cache" << std::endl;
            return false;
        }

        return true;
    }

    bool isValidStateValue(enum KeyState stateValue)
    {
        switch(stateValue) {
            case KeyState::NEUTRAL: break;
            case KeyState::PRESS:   break;
            case KeyState::HOLD:    break;
            case KeyState::RELEASE: break;
            default: // Not a valid key state. 
                std::cerr << "Invalid key state: " << getKeyStateString(stateValue) << ". Does not match a valid KeyState." << std::endl;
                return false;
        }

        return true;
    }

    bool convertStateIntToStateEnum(int32_t stateInt, enum KeyState& outState) {
        switch(stateInt) {
            case KeyState::NEUTRAL:
                outState = KeyState::NEUTRAL;
                break;
            case KeyState::PRESS:
                outState = KeyState::PRESS;
                break;
            case KeyState::HOLD:
                outState = KeyState::HOLD;
                break;
            case KeyState::RELEASE:
                outState = KeyState::RELEASE;
                break;
            default:
                outState = KeyState::INVALID;
                std::cerr << "INVALID state int.: " << std::to_string(stateInt) << " Does not match any KeyState value." << std::endl;
                // Not a valid key state. 
                return false;
        }

        return true;
    }

    bool convertActionIntToKeyState(int action, enum KeyState& outState) {
        switch(action) {
            case GLFW_PRESS:
                outState = KeyState::PRESS;
                break;
            case GLFW_RELEASE:
                outState = KeyState::RELEASE;
                break;
            case GLFW_REPEAT:
                outState = KeyState::INVALID;
                break;
            default:
                // Not a valid GLFW action. 
                std::cerr << "int param has no matching GLFW action: " << std::to_string(action) << std::endl;
                outState = KeyState::INVALID;
                return false;
        }

        return true;
    }

    std::string getKeyStateString(enum KeyState stateValue) {
        switch(stateValue) {
            case KeyState::KeyState:return "KeyState";
            case KeyState::INVALID: return "INVALID";
            case KeyState::NEUTRAL: return "NEUTRAL";
            case KeyState::PRESS:   return "PRESS";
            case KeyState::HOLD:    return "HOLD";
            case KeyState::RELEASE: return "RELEASE";
            case KeyState::COUNT:   return "COUNT";
            default: // Not a valid key state. 
                std::cerr << "INVALID state enum: " << getKeyStateString(stateValue) << ". Does not match any KeyState values." << std::endl;
                return "";
        }
    }

#pragma endregion =====================================================================================================================
}

