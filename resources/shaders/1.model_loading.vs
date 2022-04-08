#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneIds; // because MAX_BONE_INFLUENCE == 4
layout (location = 6) in vec4 weights;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 128;
const int MAX_BONE_INFLUENCE = 4;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}