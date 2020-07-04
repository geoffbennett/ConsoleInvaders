#include "bullet.h"

void bullet::update(std::vector<game_object*>& game_objects, keyboard* input, const float elapsed, game_state& state)
{
	f_delta_t_ += f_speed_ * elapsed;
	if (f_delta_t_ < 1.0f) return;

	y_--;
	if (y_ == 0) set_deleted(true);
	
	f_delta_t_ -= 1.0f;
}

void bullet::draw(console_screen* screen)
{
	screen->draw(x_, y_, L'.', fg_cyan);
}

void bullet::collided_with(std::vector<game_object*>& game_objects, game_state& state)
{
}
