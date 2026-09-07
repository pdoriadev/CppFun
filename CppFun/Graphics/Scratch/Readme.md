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
