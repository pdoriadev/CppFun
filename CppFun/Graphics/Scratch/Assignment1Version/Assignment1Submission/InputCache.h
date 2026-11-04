#ifndef INPUT_CACHE_H
#define INPUT_CACHE_H
#endif

#include <GLFW/glfw3.h>     // Window + Input Library - window/context creation, input, timing
#include <vector>
#include <iostream> // Until I figure out a good multi-threaded logging solution. Probably spdlog. 
#include <string>
#include "utils.h"

//-///////////////////////////////////////////////////
// *NOT* THREAD-SAFE
namespace InputCache {
    enum class InputType : int32_t {
        InputType = -100,
        INVALID = -1,
        KEY = 0,
        MOUSE_BUTTON,
        SCROLL,
        COUNT
    };

    enum InputState : int32_t {
        InputState = -100,
        INVALID = -1,
        NEUTRAL = 0,
        PRESS = 1,
        HOLD = 2,
        RELEASE = 3,
        COUNT
    };

    struct InputRawData {
        InputType type = InputType::INVALID;
        int32_t token = -1;
        enum InputState state = InputState::INVALID;

        InputRawData(InputType _type, int32_t _token, int _state); // convert GLFW action int to InputState on construction.
        InputRawData(InputType _type, int32_t _token, enum InputState _state);
        InputRawData(InputType _type, int32_t _token);
    };

    struct InputStateLocation{
        std::vector<enum InputState>* statesArray;
        unsigned int index = -1;

        InputStateLocation();
        InputStateLocation(std::vector<enum InputState>* _stateArr, unsigned int _index) ;
    };

#pragma region STATE_CHANGING_FUNCTIONS

    //-/////////////////////////////////////////////////////////
    // Initializes the key state cache if not initialized. 
    // returns true if cache is initialized by return. Could already be initialized.
    bool initializeCache();

    //-/////////////////////////////////////////////////////////
    // Updates key state based on captured key action.
    // param 1 - int value of a matching GLFW key macro: https://www.glfw.org/docs/3.3/group__keys.html
    // param 2 - what the key did. GLFW_PRESS or GLFW_RELEASE. GLFW_REPEAT is unreliable and will be ignored.
    // Called by external functions. 
    bool cacheInputState(InputRawData data);

    //-/////////////////////////////////////////////////////////
    // updateSingleFrameStates() - updates one-frame state to persistent state.
    // Pressed --> Hold.
    // Released --> Neutral.
    //
    // Call this after processing input in a frame so state is updated for next frame.
    bool updateSingleFrameStates();

#pragma endregion =====================================================================================================================

#pragma region STATE_ACCESSING_FUNCTIONS

    //-/////////////////////////////////////////////////////////
    // Gets cached state for keycode   
    bool getState(int32_t glfwKeyCode, enum InputState& state);

    //-///////////////////////////////////////////
    // isKeyState() returns true if the key is in the passed in state
    // param 1 - the key we care about
    // param 2 - is the key in this state?
    //
    // We just want to know if a key is in a specific state. Returns true if true.

#pragma endregion =====================================================================================================================

#pragma region HELPER_FUNCTIONS

    //-/////////////////////////////////////////////////////////
    // returns false if cache has failed to initialize.
    // Convenient wrapper at the top of cache get/set functions.
    bool isCacheInitialized();

    //-/////////////////////////////////////////////////////////
    // returns false if the enum value is not a valid state. 
    bool isValidStateValue(enum InputState stateEnum);

    //-/////////////////////////////////////////////////////////
    // sets tokenArray to the array that matches the input type.
    // returns NULL if invalid state
    std::vector<int32_t>* getTokensArray(InputType type);

    //-/////////////////////////////////////////////////////////
    // sets statesArray to the array that matches the input type.
    // returns NULL if invalid state.
    std::vector<enum InputState>* getStatesArray(InputType type);

    //-/////////////////////////////////////////////////////////
    // outs the array and index location of an input token's state
    bool getStateLocationData(InputRawData rawData, InputStateLocation& outStateData);

    //-/////////////////////////////////////////////////////////
    // outs the current state of the matching input token.
    bool getCurrentState(InputRawData rawData, enum InputState& outState);

    //-/////////////////////////////////////////////////////////
    // returns true if the given token matches the given state
    bool isInputState(InputRawData data);

    //-/////////////////////////////////////////////////////////
    // Internal Helper function. Finds index of matching key.
    bool findTokenIndex(std::vector<int32_t>& tokenArray, const int32_t inputToken, unsigned int& outTokenIndex);

    //-/////////////////////////////////////////////////////////
    // Helper function. Finds an int value's matching KeyState state. 
    bool convertStateIntToStateEnum(int32_t stateInt, enum InputState& stateEnum);

    //-/////////////////////////////////////////////////////////
    // Helper function. Finds a GLFW action int's matching KeyState state.
    bool convertActionIntToInputState(int action, enum InputState& state);

    //-/////////////////////////////////////////////////////////
    // Returns string name for enum value.
    std::string getInputStateString(enum InputState stateEnum);

    //-/////////////////////////////////////////////////////////
    // 
    bool outputTokensArray(std::vector<int32_t>& tokensArray);

    //-/////////////////////////////////////////////////////////
    // 
    bool outputStatesArray(std::vector<enum InputState>& tokensArray);



#pragma endregion =====================================================================================================================
}