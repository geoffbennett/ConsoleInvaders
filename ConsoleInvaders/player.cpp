#include "player.h"

player::player(console_screen* screen, game_timer* timer, keyboard* input) : game_object(screen, timer, input)
{
	x_ = static_cast<float>(p_screen_->get_width() / 2.0f);
	y_ = static_cast<float>(p_screen_->get_height() - 4.0f);
}

void player::update()
{
	if (p_input_->get_key(VK_LEFT).held) x_ -= f_speed_ * p_timer_->get_elapsed();
	if (p_input_->get_key(VK_RIGHT).held) x_ += f_speed_ * p_timer_->get_elapsed();
}

void player::draw()
{
	p_screen_->plot_char(x_, y_, L'#');
}
