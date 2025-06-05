#ifndef ANIMATION
#define ANIMATION

#include <vector>
#include <cstddef>

template <typename T> class Animation
{
public:
	struct Keyframe
	{
		float time;
		T value;
	};

	Animation(std::vector<Keyframe> keyframess = {}) : keyframes{ keyframess },
		duration(keyframes.empty() ? 0.0f : keyframes[keyframes.size() - 1].time) {}

	float GetDuration() const { return duration; }

	void SetKeyframe(float time, T value)
	{
		keyframes.push_back(Keyframe{ time, value });
		duration = keyframes[keyframes.size() - 1].time;
	}

	T Get(float time) const
	{
		if (keyframes.empty()) return T();
		if (keyframes.size() == 1 || time < keyframes[0].time) return keyframes[0].value;

		if (time > keyframes[keyframes.size() - 1].time) return keyframes[keyframes.size() - 1].value;

		int i = 0;
		for (int j = 0; j < keyframes.size() - 1; j++)
		{
			if (time < keyframes[j + 1].time)
			{
				i = j;
				break;
			}
		}
		
		return keyframes[i].value;
	}
private:
	std::vector<Keyframe> keyframes;
	float duration;
};

template <typename T> class Animator
{
public:
	bool is_lopping = false;

	Animator(T _base = T(), std::vector<Animation<T>> _animations = {}) :
		base{ _base }, animations{ _animations }, current{ -1 }, time{ 0.0f } {}

	void SetAnimation(int anim, bool loop = false)
	{
		if (anim == -1 || anim < animations.size())
		{
			current = anim;
			is_lopping = loop;
			time = 0.0f;
		}
	}

	void Update(float deltaT)
	{
		time += deltaT;
		if (current >= 0 && time >= animations[current].GetDuration())
		{
			if (is_lopping)
			{
				time -= animations[current].GetDuration();
			}
			else
			{
				current = -1;
			}
		}
	}

	T Get()
	{
		if (current == -1) return base;
		else return animations[current].Get(time);
	}
private:
	T base;
	std::vector<Animation<T>> animations;
	int current;
	float time;
};

#endif // !ANIMATION
