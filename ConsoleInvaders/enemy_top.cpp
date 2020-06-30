#include "enemy_top.h"

void enemy_top::draw(console_screen* screen)
{
	screen->plot_char(x_, y_, cur_);
}

void enemy_top::update(keyboard* input, float elapsed)
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

void enemy_top::collided(game_object* target)
{
}
