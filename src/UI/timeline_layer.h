#ifndef UI_IMGUI_TIMELINE_LAYER_H
#define UI_IMGUI_TIMELINE_LAYER_H

#include <vector>
#include <memory>

class Scene;
namespace glcpp
{
    class Animator;
}

namespace ui
{
    class TextEditLayer;
    class TimelineLayer
    {
    public:
        TimelineLayer();
        ~TimelineLayer() = default;
        void draw(std::vector<std::shared_ptr<Scene>> &scenes, Scene *scene, glcpp::Animator *animator);

    private:
        std::shared_ptr<TextEditLayer> text_editor_;
    };
}

#endif
