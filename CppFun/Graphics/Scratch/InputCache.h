#ifndef INPUT_CACHE_H
#define INPUT_CACHE_H
#endif

#include <GLFW/glfw3.h>     // Window + Input Library - window/context creation, input, timing
#include <vector>
#include <iostream> // Until I figure out a good multi-threaded logging solution. Probably spdlog. 
#include <string>

//-///////////////////////////////////////////////////
// *NOT* THREAD-SAFE
namespace InputCache {
    enum KeyState : int32_t {
        KeyState = -100,
        INVALID = -1,
        NEUTRAL = 0,
        PRESS = 1,
        HOLD = 2,
        RELEASE = 3,
        COUNT
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
    bool cacheKeyState(int32_t glfwKeyCode, int32_t action);

    bool updateSingleFrameStates();

#pragma endregion =====================================================================================================================

#pragma region STATE_ACCESSING_FUNCTIONS

    //-/////////////////////////////////////////////////////////
    // Gets cached state for keycode   
    bool getState(int32_t glfwKeyCode, enum KeyState& state);

    //-///////////////////////////////////////////
    // isKeyState() returns true if the key is in the passed in state
    // param 1 - the key we care about
    // param 2 - is the key in this state?
    //
    // We just want to know if a key is in a specific state. Returns true if true.
    bool isKeyState(int32_t glfwKeyCode, enum InputCache::KeyState stateValue);

#pragma endregion =====================================================================================================================

#pragma region HELPER_FUNCTIONS

    //-/////////////////////////////////////////////////////////
    // returns false if cache has failed to initialize.
    // Convenient wrapper at the top of cache get/set functions.
    bool isCacheInitialized();

    //-/////////////////////////////////////////////////////////
    // returns false if the enum value is not a valid state. 
    bool isValidStateValue(enum KeyState stateEnum);

    //-/////////////////////////////////////////////////////////
    // Internal Helper function. Finds index of matching key.
    bool findKeyCodeIndex(int32_t glfwKeyCode, int32_t& keyIndex);

    //-/////////////////////////////////////////////////////////
    // Helper function. Finds an int value's matching KeyState state. 
    bool convertStateIntToStateEnum(int32_t stateInt, enum KeyState& stateEnum);

    //-/////////////////////////////////////////////////////////
    // Helper function. Finds a GLFW action int's matching KeyState state.
    bool convertActionIntToKeyState(int action, enum KeyState& state);

    //-/////////////////////////////////////////////////////////
    // Returns string name for enum value.
    std::string getKeyStateString(enum KeyState stateEnum);


#pragma endregion =====================================================================================================================
}