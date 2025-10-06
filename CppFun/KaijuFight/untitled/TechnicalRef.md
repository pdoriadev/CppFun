
---
---
---
# MVP Technical Design
---
---
---

## Input Controller
- Captures user input. Sends it out to subscribed functions on confirmation. (how the command system works)

## Battle UI (MVC)
- Battle View (what the user sees)
- ~~Battle Controller (manages user requests, selects view response)~~
    * Subscribes to input controller
    * Interprets input and passes selection to model
- ~~Battle Model-Controller(acts on battle data)~~
    * Caches input data over time as needed.
    * Model takes 
- Battle Model-Controller
    * Subscribes to input controller
    * Interprets input and passes selection to model
    * Caches input data over time as needed.
    * Model takes 

## Debug Commands
- Reset battle
- Reset kaiju1 to full
- Reset kaiju2 to full
- Activate ability X ?

## User Actions vs. Kaiju Abilities
- User Actions 
    * MVP
        + UI move between buttons
        + UI select
    * Beyond MVP
        + UI hold-select
        + UI release hold-select
        + UI scroll? i.e. message logs
- Kaiju Abilities
    * Anything a kaiju does in battle

KAIJU Data Storage
- Const Table that can only be accessed by getter functions
- External processes access data through getter functions

## KAIJU Ability System

Function pointer example:

    #include <iostream>
    int funcTest(int test)
    {
        return test;
    }
    
    int main (int argc, char *argv[])
    {
        // Declaration
        int (*funcPtr) (int)
        // Assignment
        funcPtr = funcTest;
        // Call
        std::cout << funcPtr(0) << std::endl;
    }
    
    // outputs '0'


- **VERSION 1** - Ability function pointer to a kaiju's given ability. The function takes in battle data and mutates it.
    * Has power to change any battle state given to it. Instanced kaiju data is publically mutable. 
    * Case 1 - The ability function changes instanced kaiju data directly. How do we return to default? 
        + Any kaiju data can return to default by calling a separate Battle Ability referencing stored defaults (could be really good for debug)
    * Case 2 - Pass battle state data to kaiju 
    * Each kaiju has id's that say what ability functions to point to
- **VERSION 2** - Ability function pointer to a kaiju's given ability. The function takes in battle data and packages commands to be handed back to the battle model. The battle model processes those commands, coordinating with the battle controller/view as needed.
    * Each kaiju has id's that say what ability functions to point to
- Use-case - Not busy - Atomic Breath
    * Player selects atomic breath ablity while their kaiju is not **busy**
    * Atomic Breath ability function is called using a function pointer. References to Kaiju and other battle data are passed to the ability function (i.e. actingKaiju, receivingKaiju, distanceBetweenActingAndReceiving)
    * The ability function mutates kaiju and battle data based on input. returns bool or result( , )
        + Option 1 - On mutation, calls appropriate functions for player-facing affects?
        + Option 2 - sets outParams. Once function returns, caller resolves outParams results. 
            - switch statement? What are the permutations? 
                * take damage, change color based on damage type. OR adjust health number
                
- Abilities are stored like: command text may differ from kaiju to kaiju for same ability (i.e. move)

    differently depending on kaiju state. **Pass data to ability function**
    *

- what if I want to re-use atomic breath, but with a different range? then that's something determined by the kaiju? That should be included in the necessary data or it's default.
- then should I be passing a class? NO. not yet at least. See 
if I can get away with jsut function params. 
- function pointer cannot be const because I want monsters to be able to swap out abilities.
- kaiju passes data to be calculated. Result is determined in function? Or is result determined
    after it gets passed back (difference between calculating damage amount and straight-up applying the damage in the function). 

KAIJU **Busy** state
- **Busy** State - An action is still in progress. The user must wait until that action has completed.
- **Cancelable** attacks? - Cancel atomic breath but lose energy? Half energy?

## FX System
- Procedural
    * Cases
        + VFX like adjusting HP, Energy, Distance, etc. in different ways
        + Any text-based visuals
    * I just want a funtion I can call that updates all this. 
        + Updates over time 
            - multi-threading??
            - 
- File-based
    * Cases
        + Audio
        + Images

