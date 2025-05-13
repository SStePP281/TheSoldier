#pragma once
#ifndef RENDST
#define RENDST

#include <functional>

class RenderState
{
public:
	RenderState() = default;
	RenderState(std::function<void(float deltaTime)>&& _updateFunc,
		std::function<void()>&& _drawFunc) : 
		updateFunc{ _updateFunc }, drawFunc{ _drawFunc } {}
	~RenderState() = default;

	void update(float deltaTime) { updateFunc(deltaTime); }
	void draw() { drawFunc(); }

private:
	std::function<void(float deltaTime)> updateFunc;
	std::function<void()> drawFunc;
};

#endif // !RENDST