#include "InputCache.h"
#include <GLFW/glfw3.h>
#include "utils.h"

namespace InputCache {
    bool initialized = false;
    // parallel arrays are easier to work with. 
    std::vector<int32_t> keyTokens;
    std::vector<enum InputState> keyStates;

    std::vector<int32_t> mouseTokens;
    std::vector<enum InputState> mouseButtonStates;

    std::vector<double> mouseScrollInputs = {0.0f, 0.0f};

#pragma region INPUT_INFO_CLASS_DEFINITIONS

    //-/////////////////////////////////////////////////////////////////////////////////////////////

    InputRawData::InputRawData(InputType _type, int32_t _token, int _action) {
        type = _type;
        token = _token;
        convertActionIntToInputState(_action, state);
    }
    
    InputRawData::InputRawData(InputType _type, int32_t _token, enum InputState _state) {
        type = _type;
        token = _token;
        state = _state;
        isValidStateValue(state);
    }
    
    InputRawData::InputRawData(InputType _type, int32_t _token) {
        type = _type;
        token = _token;
    }

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    
    InputStateLocation::InputStateLocation(std::vector<enum InputState>* _stateArr, unsigned int _index) {
        statesArray = _stateArr;
        index = _index;
    }

    // Empty default constructor
    InputStateLocation::InputStateLocation() { }


#pragma endregion =====================================================================================================================

#pragma region INITIALIZATION

    bool initializeCache() {
        if (initialized) return true;

        /////////////////////////////////////////
        // INITIALIZE KEYS ARRAYS
        ////////////////////////////////////////

        keyTokens.reserve(130);
        // Add glfw tokens to array
        {
        keyTokens.emplace_back(GLFW_KEY_SPACE        );
        keyTokens.emplace_back(GLFW_KEY_APOSTROPHE   );
        keyTokens.emplace_back(GLFW_KEY_COMMA        );
        keyTokens.emplace_back(GLFW_KEY_MINUS        );
        keyTokens.emplace_back(GLFW_KEY_PERIOD       );
        keyTokens.emplace_back(GLFW_KEY_SLASH        );
        keyTokens.emplace_back(GLFW_KEY_0            );
        keyTokens.emplace_back(GLFW_KEY_1            );
        keyTokens.emplace_back(GLFW_KEY_2            );
        keyTokens.emplace_back(GLFW_KEY_3            );
        keyTokens.emplace_back(GLFW_KEY_4            );
        keyTokens.emplace_back(GLFW_KEY_5            );
        keyTokens.emplace_back(GLFW_KEY_6            );
        keyTokens.emplace_back(GLFW_KEY_7            );
        keyTokens.emplace_back(GLFW_KEY_8            );
        keyTokens.emplace_back(GLFW_KEY_9            );
        keyTokens.emplace_back(GLFW_KEY_SEMICOLON    );
        keyTokens.emplace_back(GLFW_KEY_EQUAL        );
        keyTokens.emplace_back(GLFW_KEY_A            );
        keyTokens.emplace_back(GLFW_KEY_B            );
        keyTokens.emplace_back(GLFW_KEY_C            );
        keyTokens.emplace_back(GLFW_KEY_D            );
        keyTokens.emplace_back(GLFW_KEY_E            );
        keyTokens.emplace_back(GLFW_KEY_F            );
        keyTokens.emplace_back(GLFW_KEY_G            );
        keyTokens.emplace_back(GLFW_KEY_H            );
        keyTokens.emplace_back(GLFW_KEY_I            );
        keyTokens.emplace_back(GLFW_KEY_J            );
        keyTokens.emplace_back(GLFW_KEY_K            );
        keyTokens.emplace_back(GLFW_KEY_L            );
        keyTokens.emplace_back(GLFW_KEY_M            );
        keyTokens.emplace_back(GLFW_KEY_N            );
        keyTokens.emplace_back(GLFW_KEY_O            );
        keyTokens.emplace_back(GLFW_KEY_P            );
        keyTokens.emplace_back(GLFW_KEY_Q            );
        keyTokens.emplace_back(GLFW_KEY_R            );
        keyTokens.emplace_back(GLFW_KEY_S            );
        keyTokens.emplace_back(GLFW_KEY_T            );
        keyTokens.emplace_back(GLFW_KEY_U            );
        keyTokens.emplace_back(GLFW_KEY_V            );
        keyTokens.emplace_back(GLFW_KEY_W            );
        keyTokens.emplace_back(GLFW_KEY_X            );
        keyTokens.emplace_back(GLFW_KEY_Y            );
        keyTokens.emplace_back(GLFW_KEY_Z            );
        keyTokens.emplace_back(GLFW_KEY_LEFT_BRACKET );
        keyTokens.emplace_back(GLFW_KEY_BACKSLASH    );
        keyTokens.emplace_back(GLFW_KEY_RIGHT_BRACKET);
        keyTokens.emplace_back(GLFW_KEY_GRAVE_ACCENT );
        keyTokens.emplace_back(GLFW_KEY_WORLD_1      );
        keyTokens.emplace_back(GLFW_KEY_WORLD_2      );
        keyTokens.emplace_back(GLFW_KEY_ESCAPE       );
        keyTokens.emplace_back(GLFW_KEY_ENTER        );
        keyTokens.emplace_back(GLFW_KEY_TAB          );
        keyTokens.emplace_back(GLFW_KEY_BACKSPACE    );
        keyTokens.emplace_back(GLFW_KEY_INSERT       );
        keyTokens.emplace_back(GLFW_KEY_DELETE       );
        keyTokens.emplace_back(GLFW_KEY_RIGHT        );
        keyTokens.emplace_back(GLFW_KEY_LEFT         );
        keyTokens.emplace_back(GLFW_KEY_DOWN         );
        keyTokens.emplace_back(GLFW_KEY_UP           );
        keyTokens.emplace_back(GLFW_KEY_PAGE_UP      );
        keyTokens.emplace_back(GLFW_KEY_PAGE_DOWN    );
        keyTokens.emplace_back(GLFW_KEY_HOME         );
        keyTokens.emplace_back(GLFW_KEY_END          );
        keyTokens.emplace_back(GLFW_KEY_CAPS_LOCK    );
        keyTokens.emplace_back(GLFW_KEY_SCROLL_LOCK  );
        keyTokens.emplace_back(GLFW_KEY_NUM_LOCK     );
        keyTokens.emplace_back(GLFW_KEY_PRINT_SCREEN );
        keyTokens.emplace_back(GLFW_KEY_PAUSE        );
        keyTokens.emplace_back(GLFW_KEY_F1           );
        keyTokens.emplace_back(GLFW_KEY_F2           );
        keyTokens.emplace_back(GLFW_KEY_F3           );
        keyTokens.emplace_back(GLFW_KEY_F4           );
        keyTokens.emplace_back(GLFW_KEY_F5           );
        keyTokens.emplace_back(GLFW_KEY_F6           );
        keyTokens.emplace_back(GLFW_KEY_F7           );
        keyTokens.emplace_back(GLFW_KEY_F8           );
        keyTokens.emplace_back(GLFW_KEY_F9           );
        keyTokens.emplace_back(GLFW_KEY_F10          );
        keyTokens.emplace_back(GLFW_KEY_F11          );
        keyTokens.emplace_back(GLFW_KEY_F12          );
        keyTokens.emplace_back(GLFW_KEY_F13          );
        keyTokens.emplace_back(GLFW_KEY_F14          );
        keyTokens.emplace_back(GLFW_KEY_F15          );
        keyTokens.emplace_back(GLFW_KEY_F16          );
        keyTokens.emplace_back(GLFW_KEY_F17          );
        keyTokens.emplace_back(GLFW_KEY_F18          );
        keyTokens.emplace_back(GLFW_KEY_F19          );
        keyTokens.emplace_back(GLFW_KEY_F20          );
        keyTokens.emplace_back(GLFW_KEY_F21          );
        keyTokens.emplace_back(GLFW_KEY_F22          );
        keyTokens.emplace_back(GLFW_KEY_F23          );
        keyTokens.emplace_back(GLFW_KEY_F24          );
        keyTokens.emplace_back(GLFW_KEY_F25          );
        keyTokens.emplace_back(GLFW_KEY_KP_0         );
        keyTokens.emplace_back(GLFW_KEY_KP_1         );
        keyTokens.emplace_back(GLFW_KEY_KP_2         );
        keyTokens.emplace_back(GLFW_KEY_KP_3         );
        keyTokens.emplace_back(GLFW_KEY_KP_4         );
        keyTokens.emplace_back(GLFW_KEY_KP_5         );
        keyTokens.emplace_back(GLFW_KEY_KP_6         );
        keyTokens.emplace_back(GLFW_KEY_KP_7         );
        keyTokens.emplace_back(GLFW_KEY_KP_8         );
        keyTokens.emplace_back(GLFW_KEY_KP_9         );
        keyTokens.emplace_back(GLFW_KEY_KP_DECIMAL   );
        keyTokens.emplace_back(GLFW_KEY_KP_DIVIDE    );
        keyTokens.emplace_back(GLFW_KEY_KP_MULTIPLY  );
        keyTokens.emplace_back(GLFW_KEY_KP_SUBTRACT  );
        keyTokens.emplace_back(GLFW_KEY_KP_ADD       );
        keyTokens.emplace_back(GLFW_KEY_KP_ENTER     );
        keyTokens.emplace_back(GLFW_KEY_KP_EQUAL     );
        keyTokens.emplace_back(GLFW_KEY_LEFT_SHIFT   );
        keyTokens.emplace_back(GLFW_KEY_LEFT_CONTROL );
        keyTokens.emplace_back(GLFW_KEY_LEFT_ALT     );
        keyTokens.emplace_back(GLFW_KEY_LEFT_SUPER   );
        keyTokens.emplace_back(GLFW_KEY_RIGHT_SHIFT  );
        keyTokens.emplace_back(GLFW_KEY_RIGHT_CONTROL);
        keyTokens.emplace_back(GLFW_KEY_RIGHT_ALT    );
        keyTokens.emplace_back(GLFW_KEY_RIGHT_SUPER  );
        keyTokens.emplace_back(GLFW_KEY_MENU         );
        keyTokens.emplace_back(GLFW_KEY_LAST         );
        }
        keyTokens.shrink_to_fit();

        keyStates.reserve(keyTokens.size());
        for (unsigned int i = 0; i < keyTokens.size(); ++i) {
            keyStates.emplace_back(InputState::NEUTRAL);
        }
        keyStates.shrink_to_fit();

        std::cout << "Key States Size = " << keyStates.size() << std::endl;
        
        /////////////////////////////////////////
        // INITIALIZE MOUSE BUTTONS ARRAYS
        ////////////////////////////////////////
       
        mouseTokens.reserve(10);
        // Add glfw tokens to array
        {
            mouseTokens.emplace_back(GLFW_MOUSE_BUTTON_1);
            mouseTokens.emplace_back(GLFW_MOUSE_BUTTON_2);
            mouseTokens.emplace_back(GLFW_MOUSE_BUTTON_3);
            mouseTokens.emplace_back(GLFW_MOUSE_BUTTON_4);
            mouseTokens.emplace_back(GLFW_MOUSE_BUTTON_5);
            mouseTokens.emplace_back(GLFW_MOUSE_BUTTON_6);
            mouseTokens.emplace_back(GLFW_MOUSE_BUTTON_7);
            mouseTokens.emplace_back(GLFW_MOUSE_BUTTON_8);
        }
        mouseTokens.shrink_to_fit();

        mouseButtonStates.reserve(mouseTokens.size());
        for (unsigned int i = 0; i < mouseTokens.size(); ++i) {
            mouseButtonStates.emplace_back(InputState::NEUTRAL);
        }
        mouseButtonStates.shrink_to_fit();

        std::cout << "Mouse states Size = " << mouseButtonStates.size() << std::endl;

        initialized = true;
        return true;
    }

#pragma endregion =====================================================================================================================

#pragma region STATE_CHANGING_FUNCTIONS

    bool isValidInputType(InputType type){
        switch(type){
            case InputType::KEY:
                return true;
            case InputType::MOUSE_BUTTON:
                return true;
            case InputType::SCROLL:
                return true;
            default:
                return false;
        }
        return true;
    }

    bool cacheInputState(InputRawData data) {
        if (isCacheInitialized() == false) return false;
        
        if (data.state == InputState::INVALID) {
            if (data.state != GLFW_REPEAT) {
                std::cerr << "Attempted to cache INVALID GLFW action." << std::endl;
            }
            else {
                //std::cerr << "NOT using GLFW_REPEAT. Unreliable. See: https://www.glfw.org/docs/3.3/input_guide.html" << std::endl;
            }

            return false;
        }

        InputStateLocation stateLoc;
        getStateLocationData(data, stateLoc);
        
        enum InputState& cachedStateReference = (*stateLoc.statesArray)[stateLoc.index];
        enum InputState cachedStateValue = (*stateLoc.statesArray)[stateLoc.index];;

        switch(cachedStateValue) {
            case InputState::NEUTRAL:
                cachedStateReference = data.state; // new state *should* be press. Make a check??
                break;
            case InputState::PRESS:
                if (data.state == InputState::PRESS) { cachedStateReference= InputState::HOLD; }
                else                                { cachedStateReference= InputState::RELEASE; }
                break;
            case InputState::HOLD:
                if (data.state == InputState::PRESS) { /* Do nothing */ }
                else if (data.state == InputState::RELEASE) {cachedStateReference = InputState::RELEASE; }
                break;
            case InputState::RELEASE:
                if (data.state == InputState::PRESS) { cachedStateReference = InputState::PRESS; }
                else                                { cachedStateReference = InputState::NEUTRAL; }
                break;
            default:
                // Cached state is INVALID. Should NEVER happen.
                std::cerr << "INVALID CACHED STATE: " << std::to_string(cachedStateValue) << " Action State: " << std::to_string(data.state) << std::endl;
                return false;
        }

        // std::cout << "Key " << data.token 
        //         << ". type: " << static_cast<int32_t>(data.type) 
        //         << ". newCachedState: " << getInputStateString(cachedStateReference)
        //         << ". stateValue: " << getInputStateString(data.state) << std::endl;

        return true;
    }

    bool updateSingleFrameStates() {
        for (unsigned int i = 0; i < keyStates.size(); ++i) {
            enum InputState prevState = keyStates[i]; 

            // state is validated in other functions. Not checking all states here.
            switch(keyStates[i])
            {
                case InputState::PRESS: { keyStates[i] = InputState::HOLD; break;}
                case InputState::RELEASE: { keyStates[i] = InputState::NEUTRAL; break;}
                default: break;
            }

            // Logging
            // if (prevState != keyStates[i]) {
            //     std::cout << "updated single frame state \n\t"
            //                 << "Previous State = " << getInputStateString(prevState) << ". \n\t" 
            //                 << "Current State = " << getInputStateString(keyStates[i]) << std::endl; 
            // }
        }

        for (unsigned int i = 0; i < mouseButtonStates.size(); ++i) {
            // state is validated in other functions. Not checking all states here.
            switch(mouseButtonStates[i])
            {
                case InputState::PRESS: { mouseButtonStates[i] = InputState::HOLD; break;}
                case InputState::RELEASE: { mouseButtonStates[i] = InputState::NEUTRAL; break;}
                default: break;
            }
        }

        return true;
    }

#pragma endregion =====================================================================================================================

#pragma region STATE_ACCESSING_FUNCTIONS

    bool getStateLocationData(InputRawData rawData, InputStateLocation& outStateData) {
        if (isCacheInitialized() == false) return false;

        std::vector<int32_t>* tokensArr;
        if ((tokensArr = getTokensArray(rawData.type)) == NULL) return false;
        if (findTokenIndex(*tokensArr, 
                        rawData.token, 
                        outStateData.index) == false ) return false;
        
        if ((outStateData.statesArray = getStatesArray(rawData.type)) == NULL) return false;

        return true;
    }

    bool getCurrentState(InputRawData rawData, enum InputState& outState) {
        if (isCacheInitialized() == false) return false;

        InputStateLocation stateData;
        getStateLocationData(rawData, stateData);

        enum InputState cachedState = (*stateData.statesArray)[stateData.index];
        if (isValidStateValue(cachedState) == false) return false;
        outState = cachedState;

        // Logging update when state changes
        // if (outState != InputState::NEUTRAL)
        // {
        //     std::cout << "GETTING STATE: " << getInputStateString(outState) 
        //                 << " for input " << rawData.token 
        //                 << " of type " << static_cast<int32_t>(rawData.type) << std::endl;
        // }
        
        return true;
    }

    bool isInputState(InputRawData data) {
        if (isCacheInitialized() == false) return false;

        enum InputState outCurrentState;
        getCurrentState(data, outCurrentState);

        if (outCurrentState == data.state) return true;
        else                        return false;
    }

    bool findTokenIndex(std::vector<int32_t>& tokenArray, const int32_t inputToken, unsigned int& outTokenIndex) {
        if (isCacheInitialized() == false) return false;

        for (unsigned int i = 0; i < tokenArray.size(); ++i) {
            if (inputToken == tokenArray[i]) {
                outTokenIndex = i;
                return true;
            }
        }

        // Could not find keycode
        std::cerr << "FAILED to find key matching value: " << inputToken;
        return false;
    }

#pragma endregion =====================================================================================================================

#pragma region HELPER_FUNCTIONS

    bool isCacheInitialized() {
        if (initializeCache() == false)
        {
            std::cerr << "FAILED to initialize cache" << std::endl;
            return false;
        }

        return true;
    }

    bool isValidStateValue(enum InputState const stateValue) {
        switch(stateValue) {
            case InputState::NEUTRAL: break;
            case InputState::PRESS:   break;
            case InputState::HOLD:    break;
            case InputState::RELEASE: break;
            default: // Not a valid key state. 
                std::cerr << "Invalid key state: " << getInputStateString(stateValue) << ". Does not match a valid KeyState." << std::endl;
                return false;
        }
        return true;
    }

    std::vector<int32_t>* getTokensArray(InputType type) {
        switch(type) {
            case InputType::KEY:
                return &keyTokens;
            case InputType::MOUSE_BUTTON:
                return &mouseTokens;
            case InputType::SCROLL:
                // std::cout << "SCROLL is not implemented" << std::endl;
                return NULL;
            default:
                std::cout << "Invalid input type." << std::endl;
                return NULL;
        }
    }

    std::vector<enum InputState>* getStatesArray(InputType type) {
        switch(type) {
            case InputType::KEY:
                return &keyStates;
            case InputType::MOUSE_BUTTON:
                return &mouseButtonStates;
            case InputType::SCROLL:
                // std::cout << "SCROLL is not implemented" << std::endl;
                return NULL;
            default:
                std::cout << "Invalid input type." << std::endl;
                return NULL;
        }
    }

    bool convertStateIntToStateEnum(int32_t const stateInt, enum InputState& outState) {
        switch(stateInt) {
            case InputState::NEUTRAL:
                outState = InputState::NEUTRAL;
                break;
            case InputState::PRESS:
                outState = InputState::PRESS;
                break;
            case InputState::HOLD:
                outState = InputState::HOLD;
                break;
            case InputState::RELEASE:
                outState = InputState::RELEASE;
                break;
            default:
                outState = InputState::INVALID;
                std::cerr << "INVALID state int: " << std::to_string(stateInt) << " Does not match any KeyState value." << std::endl;
                // Not a valid key state. 
                return false;
        }

        return true;
    }

    bool convertActionIntToInputState(int const action, enum InputState& outState) {
        switch(action) {
            case GLFW_PRESS:
                outState = InputState::PRESS;
                break;
            case GLFW_RELEASE:
                outState = InputState::RELEASE;
                break;
            case GLFW_REPEAT:
                outState = InputState::INVALID;
                break;
            default:
                // Not a valid GLFW action. 
                std::cerr << "int param has no matching GLFW action: " << std::to_string(action) << std::endl;
                outState = InputState::INVALID;
                return false;
        }

        return true;
    }

    std::string getInputStateString(enum InputState const stateValue) {
        switch(stateValue) {
            case InputState::InputState:return "InputState";
            case InputState::INVALID:   return "INVALID";
            case InputState::NEUTRAL:   return "NEUTRAL";
            case InputState::PRESS:     return "PRESS";
            case InputState::HOLD:      return "HOLD";
            case InputState::RELEASE:   return "RELEASE";
            case InputState::COUNT:     return "COUNT";
            default: // Not a valid key state. 
                std::cerr << "INVALID state enum: " << getInputStateString(stateValue) << 
                            ". Does not match any " + getInputStateString(InputState::InputState) + " values." << std::endl;
                return "";
        }
    }

    bool outputTokensArray(std::vector<int32_t>& tokensArray) {
        std::string outString = "{ ";
        for (unsigned int i = 0; i < tokensArray.size(); ++i) { 
            outString += std::to_string(tokensArray[i]);
            if (i+1 < tokensArray.size()) { outString += ", "; }
        }
        outString += " }";

        std::cout << outString << std::endl;
        return true;
    }

    bool outputStatesArray(std::vector<enum InputState>& statesArray) {
        std::string outString = "{ ";
        for (unsigned int i = 0; i < statesArray.size(); ++i) { 
            outString += getInputStateString(statesArray[i]);
            if (i+1 < statesArray.size()) { outString += ", "; }
        }
        outString += " }";

        std::cout << outString << std::endl;
        return true;
    }

#pragma endregion =====================================================================================================================
}

