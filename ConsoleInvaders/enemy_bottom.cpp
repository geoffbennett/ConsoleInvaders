#include "enemy_bottom.h"

void enemy_bottom::draw(console_screen* screen)
{
	screen->plot_char(x_, y_, cur_);
}

void enemy_bottom::update(std::vector<game_object*>& game_objects, keyboard* input, float elapsed)
{
	if (cur_ == f1_)
	{
		cur_ = f2_;
	}
	else
	{
		cur_ = f1_;
	}
}

void enemy_bottom::collided_with(std::vector<game_object*>& game_objects)
{
	// enemies collide with bunkers or the player
}