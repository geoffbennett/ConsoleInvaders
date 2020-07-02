#include "player.h"

#include "bullet.h"

player::player(const int x, const int y) : game_object(x, y)
{
}

void player::update(std::vector<game_object*>& game_objects, keyboard* input, const float elapsed)
{
	f_delta_t_ += f_speed_ * elapsed;
	if (f_delta_t_ < 1.0f) return;

	if (input->get_key(VK_LEFT).held && x_ > 0) x_--;
	if (input->get_key(VK_RIGHT).held && x_ < 79) x_++;
	if (input->get_key(VK_SPACE).held)
	{
		auto shooting = false;
		for(auto* o : game_objects)
		{
			auto* p_b = dynamic_cast<bullet*>(o);
			if (p_b != nullptr)
			{
				shooting = true;
			}
		}
		if (!shooting)
		{
			auto* b = new bullet(x_, y_ - 1);
			game_objects.push_back(b);
		}
	}
	f_delta_t_ -= 1.0f;
}

void player::draw(console_screen* screen)
{
	screen->plot_char(x_, y_, L'\u2569');
}

void player::collided_with(std::vector<game_object*>& game_objects)
{
	// player doesn't collide with anything
}