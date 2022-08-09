#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneIds; // because MAX_BONE_INFLUENCE == 4
layout (location = 6) in vec4 weights;


uniform mat4 model;
uniform mat4 armature;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 128;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];
out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

void main()
{
    vec4 pos = armature * vec4(aPos, 1.0f);
    vec4 totalPosition = vec4(0.0f);
    vec4 totalFragPosition = vec4(0.0f);
    vec3 totalNormal = vec3(0.0f);
    float maxWeight = 0.0f;
    int maxId = 0;

    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >=MAX_BONES) 
        {
            totalPosition = pos;
            totalNormal = aNormal;
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * pos;
        mat3 bonemat3 = mat3(finalBonesMatrices[boneIds[i]]);
        vec3 localNormal = bonemat3 * aNormal;
        totalNormal += localNormal * weights[i];
        totalPosition += localPosition * weights[i];
        if(weights[i] > maxWeight) {
            maxWeight = weights[i];
            maxId = boneIds[i];
        }
   }
	
    mat4 viewModel = view * model;
    gl_Position =  projection * viewModel *totalPosition;

    TexCoords = aTexCoords;
    Normal = mat3(model)* totalNormal;
    FragPos = vec3(model * finalBonesMatrices[maxId] * pos);
}