#ifndef INPUT_CACHE_H
#define INPUT_CACHE_H
#endif

#include <GLFW/glfw3.h>     // Window + Input Library - window/context creation, input, timing
#include <unordered_map>

//-///////////////////////////////////////////////////
// *NOT* THREAD-SAFE
namespace InputCache
{
    enum KeyState : int32_t
    {
        Key_State = -100,
        INVALID = -1,
        NEUTRAL = 0,
        PRESS = 1,
        HOLD = 2,
        RELEASE = 3,
        COUNT
    };

    bool initializeCache();

    //-/////////////////////////////////////////////////////////
    // Updates key state based on captured key action.
    bool captureKeyAction(int32_t key, int32_t action);

    //-/////////////////////////////////////////////////////////
    // Helper function. Finds an int value's matching KeyState state. 
    bool convertKeyIntToKeyState(int32_t stateInt, enum KeyState& state);

    //-/////////////////////////////////////////////////////////
    // Helper function. Finds a GLFW action int's matching KeyState state.
    bool convertActionIntToKeyState(int action, enum KeyState& state);


}