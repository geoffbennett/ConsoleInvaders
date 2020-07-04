#include "enemy_bullet.h"

void enemy_bullet::update(std::vector<game_object*>& game_objects, keyboard* input, float elapsed, game_state& state)
{
	f_delta_t_ += f_speed_ * elapsed;
	if (f_delta_t_ < 1.0f) return;

	if (cur_ == f1_)
	{
		cur_ = f2_;
	}
	else if (cur_ == f2_)
	{
		cur_ = f1_;
	}

	y_++;
	if (y_ == 28) set_deleted(true);

	f_delta_t_ -= 1.0f;
}

void enemy_bullet::draw(console_screen* screen)
{
	screen->plot_char(x_, y_, cur_);
}

void enemy_bullet::collided_with(std::vector<game_object*>& game_objects, game_state& state)
{
}
