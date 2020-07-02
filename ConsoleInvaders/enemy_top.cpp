#include "enemy_top.h"

void enemy_top::draw(console_screen* screen)
{
	screen->plot_char(x_, y_, cur_);
}

void enemy_top::update(std::vector<game_object*>& game_objects, keyboard* input, float elapsed)
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

void enemy_top::collided_with(std::vector<game_object*>& game_objects)
{
	// enemies collide with bunkers or the player
}