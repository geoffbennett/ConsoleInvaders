#include "mothership.h"

#include "bullet.h"
#include "game_state.h"

void mothership::update(std::vector<game_object*>& game_objects, keyboard* input, float elapsed, game_state& state)
{
	f_delta_t_ += f_speed_ * elapsed;
	if (f_delta_t_ < 1.0f) return;

	x_++;
	if (x_ > 79)
	{
		set_deleted(true);
	}
	
	f_delta_t_ -= 1.0f;
}

void mothership::draw(console_screen* screen)
{
	if (x_ < 79) screen->draw(x_, y_, L'<', fg_red);
	if (x_ + 1 < 79) screen->draw(x_ + 1, y_, L'=', fg_red);
	if (x_ + 2 < 79) screen->draw(x_ + 2, y_, L'@', fg_red);
	if (x_ + 3 < 79) screen->draw(x_ + 3, y_, L'=', fg_red);
	if (x_ + 4 < 79) screen->draw(x_ + 4, y_, L'>', fg_red);
}

void mothership::collided_with(std::vector<game_object*>& game_objects, game_state& state)
{
	for(auto* o : game_objects)
	{
		auto* b = dynamic_cast<bullet*>(o);
		if (b != nullptr)
		{
			const auto bullet_x = b->get_x();
			const auto bullet_y = b->get_y();
			if ((y_ == bullet_y) && (bullet_x >= x_) && (bullet_x <= x_+5))
			{
				state.score += 500;
				set_deleted(true);
			}
		}
	}
}
