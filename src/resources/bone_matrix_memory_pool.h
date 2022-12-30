#ifndef RESOURCES_BONE_MATRIX_MEMORY_POOL_H
#define RESOURCES_BONE_MATRIX_MEMORY_POOL_H

#include <vector>
#include <string>

namespace anim
{
    using mat4 = int;
    /**
     * @brief for instance rendering of Bone bipyramid
     *
     */
    struct Bone
    {
        mat4 mat;
        int parent_idx;
        std::string name;
    };

    struct Skeleton
    {
        std::vector<Bone> bindpose;
    };

    struct BoneMatrix
    {
        mat4 mat;
        void *user_ptr;
    };

    class BoneMatrixMemoryPool
    {
        void clear(int begin_id, int end_id);
        std::vector<BoneMatrix> mats;
    };
}

#endif