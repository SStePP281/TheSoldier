#pragma once
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
	float getDuration() const { return duration; }

	void setKeyframe(float time, T value)
	{
		keyframes.push_back(Keyframe{ time, value });
		duration = keyframes[keyframes.size() - 1].time;
	}

	T get(float time) const
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
	bool isLopping = false;

	Animator(T _base = T(), std::vector<Animation<T>> _animations = {}) :
		base{ _base }, animations{ _animations }, current{ -1 }, time{ 0.0f } {}

	void setAnimation(int anim, bool loop = false)
	{
		if (anim == -1 || anim < animations.size())
		{
			current = anim;
			isLopping = loop;
			time = 0.0f;
		}
	}

	void update(float deltaT)
	{
		time += deltaT;
		if (current >= 0 && time >= animations[current].getDuration())
		{
			if (isLopping)
			{
				time -= animations[current].getDuration();
			}
			else
			{
				current = -1;
			}
		}
	}

	T get()
	{
		if (current == -1) return base;
		else return animations[current].get(time);
	}
private:
	T base;
	std::vector<Animation<T>> animations;
	int current;
	float time;
};
#endif // !ANIMATION

