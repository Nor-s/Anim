#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneIds; // because MAX_BONE_INFLUENCE == 4
layout (location = 6) in vec4 weights;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};
uniform mat4 model;
flat out int boneId;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
void main()
{
    TexCoords = aTexCoords;    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Normal = mat3(model)* aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
    boneId = 0;
}