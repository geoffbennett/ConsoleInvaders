#include "enemy_bottom.h"

void enemy_bottom::draw(console_screen* screen)
{
	screen->plot_char(x_, y_, cur_);
}

void enemy_bottom::update(keyboard* input, float elapsed)
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

void enemy_bottom::collided(game_object* target)
{
}
