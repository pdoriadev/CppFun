# Name: Peter Doria
# OS: Ubuntu 26.04 LTS through Windows' WSL

# Collaboration: Solo.

# References:
**Learnopengl.com pages:**
- Introduction
- Context Creation
- Drawing polygons
- Transformations
**glfw.org docs**               
- *https://glfw.org/docs*. 
- See code comments for specific references.
**OpenGL Refpages**             
- *https://registry.khronos.org/OpenGL-Refpages/*. 
- See code comments for specific references.
**VAO / VBO Tutorial**          
- *https://wikis.khronos.org/opengl/Tutorial2:_VAOs,_VBOs,_Vertex_and_Fragment_Shaders_(C_/_SDL)*.
**Common Mistakes for OpenGL**
- *https://wikis.khronos.org/opengl/Common_Mistakes*
- This was great. Found the MessageCallback function through this. 
**glm manual**
- *https://glm.g-truc.net/0.9.8/glm-0.9.8.pdf*
**Transformations Guide**
- *open.gl/transformations*
**cppreference.com**
- *https://en.cppreference.com/*

# Why does a cube need more than 8 vertices once normals are accounted for? What did you conclude, and how did it shape your vertices array?
Because we are using the DrawArray function, each vertex needs its own normal for front/back-face and lighting calculations. Each point recieved 6 floats for each triangle to account for position and normal direction. That's a lot of vertices! The configuration of the vertices array was accounted for when setting up the VAO's vertexAttribPointer config. If we messed that up, it could not point to VBO data correctly.

# Problems: 
The cube might look rectangular on your screen. 
- It was rectangular instead of square until I set the window/viewport's width/height to match my display. Is there a good way to detect a user's screen resolution or aspect ratio?

# Comments: 
No AI used (except some auto-generated browser answers).
***Review build instructions*** for full set of source files. You can copy/paste the command line instruction into termainal.

This assignent was great! I had a lot of fun.

**I implemented the smooth color transition for extra credit.**

I didn't quite get the Camera Motion Input Handler in yet, although it does detect mouse input! It outputs to console on left click.


I think the debug MessageCallback saved me a lot of time. Would probably help other students. I found it through the **Common Mistakes for OpenGL** page (see above). 
-------------------------------
### BUILD INSTRUCTIONS - Include additional source files.
Command Line Instruction: *g++ main.cpp glad.c Logging.cpp Modeling.cpp InputCache.cpp utils.cpp -o ./assignment_1.exe*

*Logging.cpp* - Used by main.cpp to log to console and to a log.txt file.
*Modeling.cpp* - Used by main.cpp to build array vertices for triangles and cubes.
*InputCache.cpp* - Used by main.cpp to update keyboard keys state (i.e. press, release, hold, neutral). Used GLFW's keyboard token values.
*utils.cpp* - helper function - IsNullPtr(void \*ptr, std::string).
-------------------------------
### Additional Questions
 
Are '-lglfw', '-lGL', and '-ldl' linking glfw and openGL libraries? I don't know what '-ldl' could mean after googling.

Do you recommend a good logging library?

Do you get a cleaner topology of triangles by arranging vertices to be orthgonal to each other? If so, is there a theorem or some other mathematical basis I can look into?


