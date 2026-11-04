# Name: Peter Doria
------------------------------
# OS: Ubuntu 26.04 LTS through Windows' WSL
------------------------------
# Collaboration: Solo.
------------------------------
# References:
**cppreference.com**
- *https://en.cppreference.com/*

------------------------------
# moveKey() - why return a new index?
To update the GUI renderer side. Can provide feedback on keyframe data as it is dragged.

------------------------------
# What's gimbal lock?
Gimbal lock happens when a rotational basis vector A becomes equal with another rotational basis B.
When two vectors are equal to each other, they are not linearly independent, thereby, we lose an axis of rotation.  

------------------------------
# Problems: 
- Animation data for imported obj's is lost somewhere between exiting the application and re-opening the scene.
- On rare occurrence, moving a key A past key B will delete key B. I think this is related to moveKey being called every frame instead of being called when mouse input is released. 

------------------------------
# Shapes Animation Breakdown
**Cube** - Loops. Rotates positively round y-axis. Moves up and down. Uses Ease-in and Ease-in-out. 
**Tessellated Sphere** - Loops. 4 key-frames. Step (translation, rotation), Linear (translation, rotation), Ease-In (translation, rotation), and Ease-out (translation, rotation).  
**Mario** - Loops. Linear (translation, scale), Linear (translation, rotation, scale). 
**Pyramid** - No Loop. 5 key-frames. Rotates negatively around y-axis with linear ease. Then rotates around y-axis with ease-out. 
**Bunny** - Loops. Scales from 0.1 to 0.3, rotates around y-axis with a linear ease. 
**Teapot** - No Loop. Rotates around y-axis with an Ease In-Out.  

------------------------------
# Comments: 
All editor source code, including edited files, are in the zip.
Run ./editor from the editor-484 directory to run the program. 

