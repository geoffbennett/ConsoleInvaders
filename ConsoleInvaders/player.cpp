#include "player.h"

player::player(const int x, const int y) : game_object(x, y)
{
}

void player::update(keyboard* input, const float elapsed)
{
	f_delta_t_ += f_speed_ * elapsed;
	if (f_delta_t_ < 1.0f) return;

	if (input->get_key(VK_LEFT).held && x_ > 0) x_--;
	if (input->get_key(VK_RIGHT).held && x_ < 79) x_++;
	f_delta_t_ -= 1.0f;
}

void player::draw(console_screen* screen)
{
	screen->plot_char(x_, y_, L'\u2569');
}

void player::collided(game_object* target)
{
}
