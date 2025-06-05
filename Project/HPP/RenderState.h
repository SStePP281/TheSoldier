#ifndef RENDST
#define RENDST

#include <functional>

class RenderState
{
public:
	RenderState() = default;
	RenderState(std::function<void(float delta_time)>&& _updateFunc,
		std::function<void()>&& _drawFunc) : 
		update_func{ _updateFunc }, draw_func{ _drawFunc } {}
	~RenderState() = default;

	void Update(float delta_time) { update_func(delta_time); }
	void Draw() { draw_func(); }

private:
	std::function<void(float deltaTime)> update_func;
	std::function<void()> draw_func;
};

#endif // !RENDST
