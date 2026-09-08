# Extended Notes
Khronos writes an OpenGL specification.
Hardware manufacturers write driver functions that implement the OpenGL specification.
A 'glue' library (i.e. GLAD) finds the driver's OpenGL functions. It has function pointers point to the OpenGL functions. 
A program uses the 'glue' library to indirectly call OpenGL functions on the hardware. - (i.e. GLFW)
Programs like GLFW create an OpenGL context - state machine? 
- Does it manage the state machine indirectly? Or it has the whole thing?

# Operating on the 'screen' or 'buffer'
Functions like glClearColor or glClear *set* or *use* the buffer that is not currently rendered to the screen.

glfwSwapBuffers swaps these buffers.


# Vertex Data Stages
### **Vertex Shader** --> **Geometry Shader** --> Shape Assembly --> Rasterization --> **Frag Shader** --> Tests and Blending
We write Vert, Geo, and Frag Shaders.
**Vertex Shader** - operates on vertices of a triangle. *MAYBE NOT ALWAYS A TRIANGLE. SEE PRIMITIVE HINTS 'GL_POINTS', 'GL_TRIANGLES', AND 'GL_LINE_STRIP'
- Each vert has position and color.*
**Geometry Shader** - Optional. Takes a primitive as input. Outputs more geo. 
**Shape/Primitive Assembly** 

Frag Shaders come after Rasterization - which is interesting! So it's manipulating color values after they are drawn??

# Making a shader

shader source. Needs to be turned into a const char*

# RAMBLES
GLSL and using OpenGL is very C-like. 
Using const char* strings for shader source data.
The pattern of creating a buffer, inputting data into the buffer, then modding the data. Reminds me of using char* strings in C. Initialize a buffer. Write input into the buffer. Then modify it as needed. *We are managing arrays*. 


# "Copied and updated from ..."
Citing where I got code if I directly copied it. 
i.e. Copied and updated from Assignment_0 by Professor Garcia-Gomez

---

# QUESTIONS
Do NDC and Screen space coordinates map to the same space? But different origins and bases? 
Screens space coords match resolution?

