#include "enemy_bottom.h"
#include "bullet.h"
#include "game_state.h"

void enemy_bottom::draw(console_screen* screen)
{
	screen->draw(x_, y_, cur_, fg_dark_grey);
}

void enemy_bottom::update(std::vector<game_object*>& game_objects, keyboard* input, float elapsed, game_state& state)
{
	if (cur_ == f1_)
	{
		cur_ = f2_;
	}
	else if (cur_ == f2_)
	{
		cur_ = f1_;
	}
	else
	{
		set_deleted(true);
	}
}

void enemy_bottom::collided_with(std::vector<game_object*>& game_objects, game_state& state)
{
	for (auto* o : game_objects)
	{
		auto* pb = dynamic_cast<bullet*>(o);
		if (pb != nullptr)
		{
			const auto bullet_x = o->get_x();
			const auto bullet_y = o->get_y();

			if ((x_ == bullet_x) && (y_ == bullet_y))
			{
				state.score += 10;
				state.enemy_kill_count++;
				cur_ = dying_;
				pb->set_deleted(true);
			}
			
			return;
		}
	}
}