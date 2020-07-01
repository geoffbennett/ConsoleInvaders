#include "enemy_manager.h"

#include "console_screen.h"
#include "enemy_bottom.h"
#include "enemy_middle.h"
#include "enemy_top.h"
#include "game_object.h"
#include "keyboard.h"

void enemy_manager::initialise_enemies()
{
	const auto start_x = x_;
	const auto start_y = y_;
	for (auto r = 0; r < 5; r++)
	{
		for (auto c = 0; c < 11; c++)
		{
			switch (r)
			{
			case 0:
				{
					auto* et = new enemy_top(start_x + c * 4, start_y + r * 2);
					enemies_.push_back(et);
					break;
				}
			case 1:
			case 2:
				{
					auto* em = new enemy_middle(start_x + c * 4, start_y + r * 2);
					enemies_.push_back(em);
					break;
				}
			default:
				{
					auto* eb = new enemy_bottom(start_x + c * 4, start_y + r * 2);
					enemies_.push_back(eb);
					break;
				}
			}
		}
	}
}

enemy_manager::enemy_manager(const int x, const int y) : game_object(x, y)
{
	initialise_enemies();
}

void enemy_manager::update(std::vector<game_object*>* game_objects, keyboard* input, const float elapsed)
{
	f_delta_t_ += f_speed_ * elapsed;
	if (f_delta_t_ < 1.0f) return;

	auto drop = false;
	for(auto* enemy : enemies_)
	{
		const auto new_x = enemy->get_x() + n_dir_;
		if ((new_x == 79) || (new_x == 0))
		{
			drop = true;
		}
		enemy->set_x(new_x);
		enemy->update(game_objects, input, elapsed);
	}
	if (drop)
	{
		n_dir_ *= -1;
		for (auto* enemy : enemies_)
		{
			enemy->set_y(enemy->get_y() + 1);
		}
	}
	f_delta_t_ -= 1.0f;
}

void enemy_manager::draw(console_screen* screen)
{
	for (auto* enemy : enemies_)
	{
		enemy->draw(screen);
	}
}

void enemy_manager::collided_with(std::vector<game_object*>* game_objects)
{
	// enemies collide with bunkers or the player
}
