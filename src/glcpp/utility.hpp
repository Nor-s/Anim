#ifndef GLCPP_UTILITY_HPP
#define GLCPP_UTILITY_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/quaternion.h>

#include <tuple>

namespace glcpp
{
    inline glm::vec3 AiVecToGlmVec(const aiVector3D &vec)
    {
        return glm::vec3(vec.x, vec.y, vec.z);
    }
    inline aiVector3D GlmVecToAiVec(const glm::vec3 &vec)
    {
        return aiVector3D(vec.x, vec.y, vec.z);
    }
    inline glm::mat4 AiMatToGlmMat(const aiMatrix4x4 &from)
    {
        glm::mat4 to;
        // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
        to[0][0] = from.a1;
        to[1][0] = from.a2;
        to[2][0] = from.a3;
        to[3][0] = from.a4;
        to[0][1] = from.b1;
        to[1][1] = from.b2;
        to[2][1] = from.b3;
        to[3][1] = from.b4;
        to[0][2] = from.c1;
        to[1][2] = from.c2;
        to[2][2] = from.c3;
        to[3][2] = from.c4;
        to[0][3] = from.d1;
        to[1][3] = from.d2;
        to[2][3] = from.d3;
        to[3][3] = from.d4;

        return to;
    }
    inline aiMatrix4x4 GlmMatToAiMat(const glm::mat4& from)
    {
        aiMatrix4x4 to;
        // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
        to.a1 = from[0][0];
        to.a2 = from[1][0];
        to.a3 = from[2][0];
        to.a4 = from[3][0];
        to.b1 = from[0][1];
        to.b2 = from[1][1];
        to.b3 = from[2][1];
        to.b4 = from[3][1];
        to.c1 = from[0][2];
        to.c2 = from[1][2];
        to.c3 = from[2][2];
        to.c4 = from[3][2];
        to.d1 = from[0][3];
        to.d2 = from[1][3];
        to.d3 = from[2][3];
        to.d4 = from[3][3];

        return to;
    }
    static inline glm::quat AiQuatToGlmQuat(const aiQuaternion &pOrientation)
    {
        return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
    }
    static inline aiQuaternion GlmQuatToAiQuat(const glm::quat& pOrientation)
    {
        return aiQuaternion(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
    }
    //return translate, rotate, scale
    static inline std::tuple<glm::vec3, glm::quat, glm::vec3> DecomposeTransform(const glm::mat4 &transform)
    {
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(transform, scale, rotation, translation, skew, perspective);
        return {translation, rotation, scale};
    }
}

#endif