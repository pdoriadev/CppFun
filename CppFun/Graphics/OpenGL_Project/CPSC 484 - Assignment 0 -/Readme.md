# Name: Peter Doria
# OS: Ubuntu 26.04 LTS through Windows' WSL
# Collaboration: Solo.
# References: None so far. I'm going to look through the textbook and other resources!
# Problems: No blockers. Just questions I'll probably ask about in office hours or figure out later. It feels cool!
# Comments: 
I liked the name graphic. Felt like a nice "Welcome to the class." 

I don't really understand what's happening beyond project setup, but I that's alright since that's the assignment's focus.
I'm excited to look through the code!

-------------------------------
# Peter's Build Notes - Need to check the MakeFile to double check I'm right. 
### g++ main.cpp glad.c -o assignment_0 -lglfw
g++ - GNU's C++ compiler
main.cpp - source file 1
glad.c - source file 2 for GLAD. Don't know much about this yet. 
- In main.cpp' `#include <glad.h>`. OpenGL function loader.
-o - generate object file(s) based on the input source files. 
assignment_0 - The name of the generated executable?
-lglfw - an graphics API "for creating windows, contexts, and surfaces, receiving inputs and events." Used by OpenGL, OpenGL ES (what is this?) and Vulkan. https://www.glfw.org/. **confirm with prof**. 
- See `#include <GLFW/glfw3.h>` in main.cpp. 
-lGL - **I don't know what this is?** . Another library. For GLAD?? Check-out make.  
-ldl - **I dont' know**. 


-----

Editor
- using IMGUI. Great tool when don't want to make your own interface. Great library.  
