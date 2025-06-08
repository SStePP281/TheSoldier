#ifndef RENDST
#define RENDST

#include <functional>

class RenderState {
public:
	RenderState() = default;
	RenderState(std::function<void(float delta_time)>&& _updateFunc,
		std::function<void()>&& _drawFunc) : 
		update_func_{ _updateFunc }, draw_func_{ _drawFunc } {}
	~RenderState() = default;

	void Update(float delta_time) { update_func_(delta_time); }
	void Draw() { draw_func_(); }

private:
	std::function<void(float deltaTime)> update_func_;
	std::function<void()> draw_func_;
};

#endif // !RENDST
