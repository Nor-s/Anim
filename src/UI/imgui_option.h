#ifndef SRC_UI_IMGUI_OPTION_H
#define SRC_UI_IMGUI_OPTION_H

#include <vector>
#include <string>
#include <utility>
#include <map>
#include <tuple>

namespace ui
{
    // TODO: POP, check unexpected behavior
    class ImguiOption
    {
    public:
        ImguiOption() = default;
        virtual ~ImguiOption() = default;
        // push and return id, you must save this id.
        int push_flag(const std::string &name, bool init_value)
        {
            flags_.push_back({name, init_value});
            return static_cast<int>(flags_.size() - 1);
        }
        // push and return id, you must save this id.
        int push_int_property(const std::string &name, int init_value, int min_value, int max_value)
        {
            int_properties_.push_back({name, init_value, min_value, max_value});

            return static_cast<int>(int_properties_.size() - 1);
        }
        // push and return id, you must save this id.
        int push_float_property(const std::string &name, float init_value, float min_value, float max_value)
        {
            float_properties_.push_back({name, init_value, min_value, max_value});

            return static_cast<int>(float_properties_.size() - 1);
        }
        bool get_flag(int idx)
        {
            return flags_[idx].second;
        }
        int get_int_property(int idx)
        {
            return std::get<1>(int_properties_[idx]);
        }
        float get_float_property(int idx)
        {
            return std::get<1>(float_properties_[idx]);
        }
        std::vector<std::pair<std::string, bool>> &get_flags()
        {
            return flags_;
        }
        std::vector<std::tuple<std::string, int, int, int>> &get_int_properties()
        {
            return int_properties_;
        }
        std::vector<std::tuple<std::string, float, float, float>> &float_properties()
        {
            return float_properties_;
        }

        void set_int_property(int idx, const std::tuple<std::string, int, int, int> &item)
        {
            int_properties_[idx] = item;
        }

    private:
        // Button
        std::vector<std::pair<std::string, bool>> flags_;
        // SliderInt
        std::vector<std::tuple<std::string, int, int, int>> int_properties_;
        // SliderFloat
        std::vector<std::tuple<std::string, float, float, float>> float_properties_;
    };

}
#endif