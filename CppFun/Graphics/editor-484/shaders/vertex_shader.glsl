#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
// Second VBO, not interleaved into the stride-9 buffer above. A shape that
// supplies no UVs leaves this attribute disabled, in which case it reads as a
// constant and the hasTexture uniform stops the fragment shader looking at it.
layout(location = 3) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec3 FragColor;
out vec2 TexCoord;

void main() {
    FragPos = vec3(model * vec4(aPosition, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    FragColor = aColor;
    TexCoord  = aTexCoord;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
