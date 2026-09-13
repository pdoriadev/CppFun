# Name: Peter Doria
# OS: Ubuntu 26.04 LTS through Windows' WSL
# Collaboration: Solo.
# References:
Learnopengl.com pages:
- Introduction
- Context Creation
- Drawing polygons
- Transformations
glfw.org docs               
- *https://glfw.org/docs*. 
- See code comments for specific references.
OpenGL Refpages             
- *https://registry.khronos.org/OpenGL-Refpages/*. 
- See code comments for specific references.
VAO / VBO Tutorial          
- *https://wikis.khronos.org/opengl/Tutorial2:_VAOs,_VBOs,_Vertex_and_Fragment_Shaders_(C_/_SDL)*.
Common Mistakes for OpenGL  
- *https://wikis.khronos.org/opengl/Common_Mistakes*
- This was great. Found the MessageCallback function through this. 
glm manual                  
- *https://glm.g-truc.net/0.9.8/glm-0.9.8.pdf*
Transformations Guide       
- *open.gl/transformations*
cppreference.com            
- *https://en.cppreference.com/*
# Problems: 
The cube might look rectangular on your screen. 
- It was rectangular instead of square until I set the window/viewport's width/height to match my display. Is there a good way to detect a user's screen resolution or aspect ratio?

I think the debug MessageCallback saved me a lot of time. Would probably help other students. 

# Comments: 
No AI used (except some auto-generated browser answers).

Are '-lglfw', '-lGL', and '-ldl' linking glfw and openGL libraries? I don't know what '-ldl' could mean after googling.

Do you recommend a good logging library?

Do you get a cleaner topology of triangles by arranging vertices to be orthgonal to each other? If so, is there a theorem or some other mathematical basis I can look into?

-------------------------------
# BUILD INSTRUCTIONS - Include additional source files.
Command Line Instruction: *g++ main.cpp glad.c Logging.cpp Modeling.cpp InputCache.cpp -o ./assignment_1.exe*

*Logging.cpp* - Used by main.cpp to log to console and to a log.txt file.
*Modeling.cpp* - Used by main.cpp to build array vertices for triangles and cubes.
*InputCache.cpp* - Used by main.cpp to update keyboard keys state (i.e. press, release, hold, neutral). Used GLFW's keyboard token values. 