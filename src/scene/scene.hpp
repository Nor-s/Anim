#ifndef SRC_SCENE_SCENE_H
#define SRC_SCENE_SCENE_H

#include <string>
#include <memory>

class SceneContext;
class SharedResources;
namespace glcpp
{
    class Framebuffer;
}
class Scene
{
public:
    virtual void init_framebuffer(uint32_t width, uint32_t height) = 0;
    virtual void pre_draw() = 0;
    virtual void draw() = 0;
    virtual void set_size(uint32_t width, uint32_t height) = 0;
    virtual void set_delta_time(float dt) = 0;
    virtual std::shared_ptr<glcpp::Framebuffer> get_framebuffer() = 0;

protected:
    std::shared_ptr<SceneContext> context_;
    std::shared_ptr<SharedResources> resources_;
};
#endif