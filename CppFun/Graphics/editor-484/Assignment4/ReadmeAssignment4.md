# Name: Peter Doria
------------------------------
# OS: Ubuntu 26.04 LTS through Windows' WSL
------------------------------
# Collaboration: Solo.
------------------------------
# References:
**VAO, VBO, EBO refreshers**
- https://learnopengl.com/Getting-started/Hello-Triangle
- https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBufferData.xhtml
- https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
**cppreference.com**
- *https://en.cppreference.com/*
**Obj parsing**
- https://cs418.cs.illinois.edu/website/text/obj.html
------------------------------
# Problems: 
The pyramid's normals are still averaged. They don't look right. 

It feels like I'm quickly approaching low frame-rates on a high-end laptop workstation. What optimizations can I do to improve frame rates?
- The screenshot hovered around ~2 fps. It was ~10-20 fps before adding the dragon.

------------------------------
# OBJ files and import problems
- steampunkdragon.obj

OBJ import-related Problems
- Constructing a scene takes a *long* time without gizmos. Will that be part of the class? Are there resources you'd recommend to make those?
- After adding a sufficiently complex mesh, the editor crawls below 5fps. At thisframe-rate, editor buttons become unreliable (i.e. clicking on transform field to adjust values with mouse or keyboard did not work. CTRL+CLICK enables keyboard-only input, which let me work-around the issue.) 

------------------------------
# Additional Functionality
Panning - slowed the panning speed as the radius increases. 

------------------------------
# Comments: 
All editor source code, including edited files, are in the zip.
Run ./editor from the editor-484 directory to run the program. 

