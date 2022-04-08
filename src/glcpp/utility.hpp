#ifndef GLCPP_UTILITY_HPP
#define GLCPP_UTILITY_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/quaternion.h>

namespace glcpp
{
    inline glm::vec3 AiVecToGlmVec(const aiVector3D &vec)
    {
        return glm::vec3(vec.x, vec.y, vec.z);
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
    static inline glm::quat AiQuatToGlmQuat(const aiQuaternion &pOrientation)
    {
        return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
    }
}

#endif