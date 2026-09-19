#include "shaderSources.h"



#pragma region SHADER_SOURCE 

// Look under "Basic Types" for GLSL data types: https://wikis.khronos.org/opengl/Data_Type_(GLSL)

char const * getVertexShaderSource_no_color() {
    //-///////////////////////////////////////////////
    // VERT SHADER - Runs on every vertex before shape assembly
    char const * vertexShaderSource_no_color =  R"GLSL(
    #version 330 core
    // OpenGL version to run the shader
    // layout?? location??
    // in is input vector data.
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNor;

    // "out" variables are computed once per vertex here, then automatically
    // interpolated across each triangle before the fragment shader below sees
    // them (that interpolation step is called rasterization).

    out vec3 Normal; // will be picked up by "in vec3 Normal" in the fragment shader
    out vec3 lightToVertex;

    // A "uniform" is a value we set once per draw call from the CPU (see
    // glUniformMatrix4fv in the render loop) that stays constant across every
    // vertex/pixel of that draw call -- unlike aPos/aNormal, which are
    // different for every vertex.

    uniform mat4 transform;     // this model's combined rotate+scale+position matrix, set from the CPU
    uniform vec3 lightPos;      // set in render loop.

    void main()
    {
        // Note from Assignment_0
        // gl_Position is a special built-in output: OpenGL reads it to know
        // where this vertex lands on screen (in clip space).
        gl_Position = transform * vec4(aPos, 1.0);
        // mat3(transform) keeps only the rotation+scale part of the 4x4 matrix
        // (it drops the translation column), which is what you want when
        // transforming a *direction* like a normal instead of a *point*.
        Normal = mat3(transform) * aNor;

        // Used to calculate if the light is in front or behind a plane.
        lightToVertex = aPos - lightPos;
    }
    )GLSL";

    return vertexShaderSource_no_color;
}

char const * getVertexShaderSource_color() {
    //-///////////////////////////////////////////////
    // VERT SHADER - Runs on every vertex before shape assembly
    const char* vertexShaderSource_color =  R"GLSL(
    #version 330 core
    // OpenGL version to run the shader
    // layout?? location??
    // in is input vector data.
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNor;
    layout (location = 2) in vec3 color;

    // "out" variables are computed once per vertex here, then automatically
    // interpolated across each triangle before the fragment shader below sees
    // them (that interpolation step is called rasterization).

    out vec3 Normal; // will be picked up by "in vec3 Normal" in the fragment shader
    out vec3 lightToVertex;

    // A "uniform" is a value we set once per draw call from the CPU (see
    // glUniformMatrix4fv in the render loop) that stays constant across every
    // vertex/pixel of that draw call -- unlike aPos/aNormal, which are
    // different for every vertex.

    uniform mat4 transform;     // this model's combined rotate+scale+position matrix, set from the CPU
    uniform vec3 lightPos;      // set in render loop.

    void main()
    {
        // Note from Assignment_0
        // gl_Position is a special built-in output: OpenGL reads it to know
        // where this vertex lands on screen (in clip space).
        gl_Position = transform * vec4(aPos, 1.0);
        // mat3(transform) keeps only the rotation+scale part of the 4x4 matrix
        // (it drops the translation column), which is what you want when
        // transforming a *direction* like a normal instead of a *point*.
        Normal = mat3(transform) * aNor;

        // Used to calculate if the light is in front or behind a plane.
        lightToVertex = aPos - lightPos;
    }
    )GLSL";

    return vertexShaderSource_color;
}

char const * getFragmentShaderSource() {
    //-///////////////////////////////////////////////
    // FRAG SHADER - Runs after rasterization. 
    /////////////////////////////////////////////////
    char const * fragmentShaderSource = R"GLSL(
    #version 330 core

    //-/////////////////////////////
    // 'in' variables / attributes
    // in variables map to vertex shader out variables. 
    // Names have to be *exactly* the same OR
    in vec3 Normal;
    in vec3 lightToVertex;      // the light's position, set in render loop when the position changes.
    uniform vec3 color;         // this letter's current color, set from the CPU each frame
    // uniform vec3 lightDir;      // the light's direction, set in render loop when the direction changes. 

    uniform vec3 UNUSED_UNIFORM_SILENTLY_REMOVED;  // this variable is unused. It will be silently removed: https://learnopengl.com/Getting-started/Shaders

    out vec4 fragColor; // required out vec4 for frag shader. 

    void main()
    {
        vec3 N = normalize(Normal * 1);      // interpolation can shrink the length; renormalize to unit length
        vec3 lightDir = vec3(-0.2f, -0.4f, -1.0f);     // lightDir is effectively const. 
        // Is the light direction facing the plane's front-face?
        float dotLightDir = lightDir.x * N.x + 
                            lightDir.y * N.y + 
                            lightDir.z * N.z;
        // Is the light behind this fragment's vertex?
        float dotLightToVertex =    lightToVertex.x * N.x + 
                                    lightToVertex.y * N.y + 
                                    lightToVertex.z * N.z;
        
        // intensity is 0 if the light is not facing the plane.
        // intensity is 0 if the light is behind the plane.
        float intensity = max(dotLightDir, 0.0f) * max(-dotLightToVertex, 0.0f);

        fragColor = vec4(color * intensity, 1.0);
    }
    )GLSL";

    return fragmentShaderSource;
}

#pragma endregion =====================================================================================================================
