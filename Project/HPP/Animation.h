#ifndef ANIMATION
#define ANIMATION

#include <vector>
#include <cstddef>

template <typename T> 
class Animation {
public:
	struct Keyframe {
		float time;
		T value;
	};

	Animation(std::vector<Keyframe> keyframess = {}) : keyframes_{ keyframess },
		duration_(keyframes_.empty() ? 0.0f : keyframes_[keyframes_.size() - 1].time) {}

	const float GetDuration() const { return duration_; }

	void SetKeyframe(float time, T value) {
		keyframes_.push_back(Keyframe{ time, value });
		duration_ = keyframes_[keyframes_.size() - 1].time;
	}

	const T Get(float time) const {
		if (keyframes_.empty()) return T();
		if (keyframes_.size() == 1 || time < keyframes_[0].time) return keyframes_[0].value;

		if (time > keyframes_[keyframes_.size() - 1].time) return keyframes_[keyframes_.size() - 1].value;

		int i = 0;
		for (int j = 0; j < keyframes_.size() - 1; j++) {
			if (time < keyframes_[j + 1].time) {
				i = j;
				break;
			}
		}
		
		return keyframes_[i].value;
	}
private:
	std::vector<Keyframe> keyframes_;
	float duration_;
};

template <typename T>
class Animator {
public:
	bool is_lopping = false;

	Animator(T _base = T(), std::vector<Animation<T>> _animations = {}) :
		base_{ _base }, animations_{ _animations }, current_{ -1 }, time_{ 0.0f } {}

	void SetAnimation(int anim, bool loop = false) {
		if (anim == -1 || anim < animations_.size()) {
			current_ = anim;
			is_lopping = loop;
			time_ = 0.0f;
		}
	}

	void Update(float deltaT) {
		time_ += deltaT;
		if (current_ >= 0 && time_ >= animations_[current_].GetDuration()) {
			if (is_lopping) {
				time_ -= animations_[current_].GetDuration();
			}
			else {
				current_ = -1;
			}
		}
	}

	const T Get() const {
		if (current_ == -1) return base_;
		else return animations_[current_].Get(time_);
	}
private:
	T base_;
	std::vector<Animation<T>> animations_;
	int current_;
	float time_;
};

#endif // !ANIMATION
