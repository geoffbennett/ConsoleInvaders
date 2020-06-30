#include "enemy_middle.h"

void enemy_middle::draw(console_screen* screen)
{
	screen->plot_char(x_, y_, cur_);
}

void enemy_middle::update(keyboard* input, float elapsed)
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

void enemy_middle::collided(game_object* target)
{
}
