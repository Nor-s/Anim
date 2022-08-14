#pragma
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/quaternion.h>

#include <tuple>

namespace anim
{
    inline glm::vec3 AiVecToGlmVec(const aiVector3D &vec);
    inline aiVector3D GlmVecToAiVec(const glm::vec3 &vec);
    inline glm::mat4 AiMatToGlmMat(const aiMatrix4x4 &from);

    inline aiMatrix4x4 GlmMatToAiMat(const glm::mat4 &from);

    inline glm::quat AiQuatToGlmQuat(const aiQuaternion &pOrientation);
    inline aiQuaternion GlmQuatToAiQuat(const glm::quat &pOrientation);
    // return translate, rotate, scale
    inline std::tuple<glm::vec3, glm::quat, glm::vec3> DecomposeTransform(const glm::mat4 &transform);
}