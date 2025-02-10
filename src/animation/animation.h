#ifndef ANIM_ANIMATION_ANIMATION_H
#define ANIM_ANIMATION_ANIMATION_H

#include "bone.h"
// #include "retargeter.h"

#include <assimp/scene.h>

#include <string>
#include <map>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <memory>

namespace anim
{
enum AnimationType
{
	None,
	Assimp,
	Json,
	Raw
};

using AnimDataID = int;

template<class T>
T lerp_t(const T& a, const T& b, float ratio)
{
	return a * (1.0 - ratio) + b * ratio;
}

template <class T>
class AnimData
{
	//using T = float;
public:
	struct FrameInfo
	{
		uint32_t frame;
		T value;
		bool operator==(const FrameInfo& other) const
		{
			return frame == other.frame;
		}
		bool operator<(const FrameInfo& other) const
		{
			return frame < other.frame;
		}
	};
	T get_data(float time, const T& default_data) const
	{
		int	start_index = 0, end_index = -1;
		T data = default_data;
		int size = frame_datas_.size();

		if (size == 0)
		{
			return default_data;
		}

		for (int i = 0; i < size; i++)
		{
			auto& [frame_num ,value] = frame_datas_[i];
			if (frame_num < time)
			{
				start_index = i;
				continue;
			}
			end_index = i;
			break;
		}
		if (time < frame_datas_[start_index].frame)
		{
			return default_data;
		}
		if (end_index == -1 || start_index == end_index)
		{
			return frame_datas_[start_index].value;  
		}
		const auto& start = frame_datas_[start_index];
		const auto& end = frame_datas_[end_index];
		int length = end.frame - start.frame;
		float ratio = static_cast<float>(time-start.frame) / length;
		return lerp_t(start.value, end.value, ratio);
	}
	void set_data(T value, float time)
	{
		auto frame = static_cast<uint32_t>(time);
		auto it = std::lower_bound(frame_datas_.begin(), 
									frame_datas_.end(), FrameInfo{frame, {}});

		if (it == frame_datas_.end() || it->frame != frame)
		{
			auto info = FrameInfo{frame, value};
			frame_datas_.insert(it, info);
		}
		else if (it->frame == time)
		{
			it->value = value;
		}
	}
	void remove_data(float time)
	{
		auto frame = static_cast<uint32_t>(time);
		auto it = std::find_if(frame_datas_.begin(), frame_datas_.end(), [frame](const auto& info) { return info.frame == frame;
			});
		if (it != frame_datas_.end())
		{
			frame_datas_.erase(it);
		}
	}
	const std::vector<FrameInfo>& get_all_datas() const
	{
		return frame_datas_;
	}

private:
	std::vector<FrameInfo> frame_datas_;
};

class Animation
{
	friend class MixamoRetargeter;

public:
	Animation() = default;
	Animation(std::string_view file_path);
	virtual ~Animation() = default;
	virtual glm::mat4 get_bone_local_transform(const std::string_view name,
											   const float time,
											   const float factor,
											   bool bIsRemoveTranslation);
	float get_fps();
	float get_duration();
	float get_current_duration();

	const std::string& get_name() const;
	const char* get_path() const;
	const std::map<std::string, std::unique_ptr<Bone>>& get_name_bone_map() const;
	std::map<std::string, std::unique_ptr<Bone>>& get_mutable_name_bone_map();
	const std::unordered_map<AnimDataID, AnimData<float>>& get_morph_datas() const
	{
		return morph_datas_;
	}  
	const AnimationType& get_type() const;

	virtual void reload();
	void get_ai_animation(aiAnimation* ai_anim, const aiNode* ai_root_node, float factor = 1.0, bool is_linear = true);
	void set_id(int id);
	const int get_id() const;
	void insert_or_update_keyframe(const std::string& name, const glm::mat4& transform, float time);
	void insert_or_update_morph(const AnimDataID index, const float weight, const float time);
	void remove_keyframe(const std::string& name, float time);
	void remove_morph(const AnimDataID& index, float time);
	void remove_morph_index(const AnimDataID& index);
	void update_keyframe(const std::string& name, const glm::mat4& transform, float time);

private:
	Bone* find_bone(const std::string_view name);

protected:
	float duration_{0.0f};
	int fps_{0};
	std::string name_{};
	std::map<std::string, std::unique_ptr<Bone>> name_bone_map_{};
	std::map<std::string, glm::mat4> name_bindpose_map_{};
	AnimationType type_{};
	std::unordered_map<AnimDataID, AnimData<float>> morph_datas_;
	std::string path_{};
	int id_{-1};
};

}	 // namespace anim

#endif