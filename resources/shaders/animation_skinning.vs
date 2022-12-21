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
// uniform bool isOutline;
// uniform float outlineWidth=1.0;


const int MAX_BONES = 128;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];
out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
flat out int boneId;

void main()
{
    vec4 pos = vec4(aPos, 1.0f);
    vec3 norm = aNormal;
    vec4 totalPosition = vec4(0.0f);
    vec4 totalFragPosition = vec4(0.0f);
    vec3 totalNormal = vec3(0.0f);

    float maxWeight = 0.0;
    int maxBoneId = boneIds[0];

    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >=MAX_BONES) 
        {
            totalPosition = pos;
            totalNormal = norm;
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * pos;
        mat3 bonemat3 = mat3(finalBonesMatrices[boneIds[i]]);
        vec3 localNormal = bonemat3 * norm;
        totalNormal += localNormal * weights[i];
        totalPosition += localPosition * weights[i];
        if(maxWeight < weights[i]) {
            maxWeight = weights[i];
            maxBoneId = boneIds[i];
        }
   }
	
    gl_Position =  projection * view * model * totalPosition;
    Normal = mat3(model)* totalNormal;
    TexCoords = aTexCoords;
    FragPos = vec3(model * totalPosition);
    boneId = maxBoneId;

    // if(isOutline)
    //     gl_Position =  projection * view * model *vec4(outlineWidth*Normal +vec3(totalPosition), 1.0);

}