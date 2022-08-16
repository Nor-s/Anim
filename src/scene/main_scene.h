#ifndef SRC_SCENE_MAIN_SCENE_H
#define SRC_SCENE_MAIN_SCENE_H

#include "scene.hpp"

#include <vector>
namespace anim
{
    class Entity;
    class Image;
}
class MainScene : public Scene
{
public:
    MainScene() = delete;
    MainScene(uint32_t width, uint32_t height, std::shared_ptr<anim::SharedResources> resources = nullptr);
    virtual ~MainScene() = default;
    virtual void init_framebuffer(uint32_t width, uint32_t height) override;
    virtual void pre_draw() override;
    virtual void draw() override;

private:
    void init_camera();
    void update_framebuffer();
    void draw_to_framebuffer();

    glm::vec4 background_color_{0.4f, 0.4f, 0.4f, 1.0f};

    std::shared_ptr<anim::Image> grid_framebuffer_;
};

#endif