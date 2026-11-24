# Name: Peter Doria
------------------------------
# OS: Ubuntu 26.04 LTS through Windows' WSL
# Machine - Raider 18 HX AI A2XW
# CPU - Intel Core Ultra 9 285HX (24 CPUs), ~2.8GHz
# GPU - NVIDIA RTX5090 Laptop GPU
------------------------------
# Collaboration: Solo.
------------------------------
# References:
**cppreference.com**
- *https://en.cppreference.com/*

------------------------------
# Why cube() returns one UV coordinate per triangle corner
A corner's uv depends on which face it is part of. 
The number of uvs is a function of the total indices in faces (faces.size() * 3) because each and every index needs a corresponding uv for its face.

------------------------------
# A description of what the Teapot's wrap seam looks like before it is repaired, and why the repair cannot be represented using one UV coordinate per vertex.
Before Repair - Before the wrap seam is fixed, a triangle's uv's may stretch across a whole texture. I.e. corner0's u is 0.95 while corner1's u is at 0.3. corner0 and corner1 are physically 0.35f apart in terms of u, but there's no way to mathematically show that with 0.95 and 0.3f. 
To Repair - To fix this, you can wrap the lower value, 0.3f, by adding 1.0f. When interpreting this with radians, it will reach the same position on the unit circle.
- We can't do this fix because a corner gets wrapped 'per face'. A vertex's uv can't be wrapped an not wrapped. That means our uv array can't correspond to our vertices. Each uv corresponds to each index in each face of the faces array. 

------------------------------
# An explanation of why nearest-neighbor sampling may be difficult to notice on a photograph such as 2k_earth_daymap.jpg but obvious on checker.png. Explain why the tests sample between texel centers
A 2k_earth_daymap.jpg has a lot of detail and color variation. A nearest-neighbor sample will blend in the colors. Textures like checker.png will give a bunch of white and black dots throughout the image.

------------------------------
# Problems:
Lots of logic problems on my side.
- Not generating enough face indices because of my setupCube algorithm. Appeared to work fine until I applied textures. 

------------------------------
# Comments: 
All editor source code, including edited files, are in the zip.
Run ./editor from the editor-484 directory to run the program. 

Code compiles and runs on my machine. Please follow-up with me if you run into issues. Let me know if I can send you any helpful info, hop on a call, come to office hours, etc.

