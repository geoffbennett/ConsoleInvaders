#include "enemy_manager.h"

#include "console_screen.h"
#include "enemy_bottom.h"
#include "enemy_bullet.h"
#include "enemy_middle.h"
#include "enemy_top.h"
#include "game_object.h"
#include "keyboard.h"
#include "game_state.h"
#include "player.h"

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

void enemy_manager::update_speed_mod(game_state& state) const
{
	if (state.enemy_kill_count >= 10 && state.enemy_kill_count < 20)
	{
		state.enemy_speed_mod = 3.0;
	}
	if (state.enemy_kill_count >= 20 && state.enemy_kill_count < 30)
	{
		state.enemy_speed_mod = 4.0;
	}
	if (state.enemy_kill_count >= 30 && state.enemy_kill_count < 40)
	{
		state.enemy_speed_mod = 5.0;
	}
	if (state.enemy_kill_count >= 40 && state.enemy_kill_count < 50)
	{
		state.enemy_speed_mod = 6.0;
	}
	if (state.enemy_kill_count >= 50 && state.enemy_kill_count < 60)
	{
		state.enemy_speed_mod = 8.0;
	}
	if (enemies_.size() == 1)
	{
		state.enemy_speed_mod = 12.0;
	}
}

void enemy_manager::update(std::vector<game_object*>& game_objects, keyboard* input, const float elapsed, game_state& state)
{
	f_delta_t_ += state.enemy_speed_mod * elapsed;
	if (f_delta_t_ < 1.0f) return;

	player* p = nullptr;
	for (auto* o : game_objects)
	{
		p = dynamic_cast<player*>(o);
		if (p != nullptr) break;
	}

	auto enemy_x = 0;
	auto enemy_y = p != nullptr ? p->get_y() : 0;
	game_object* shooting_enemy = nullptr;

	auto drop = false;
	for (auto* enemy : enemies_)
	{
		const auto new_x = enemy->get_x() + n_dir_;
		if ((new_x == 79) || (new_x == 0))
		{
			drop = true;
		}

		enemy->set_x(new_x);
		enemy->update(game_objects, input, elapsed, state);

		if (p != nullptr)
		{
			if ((enemy->get_x() > p->get_x() - 1) && (enemy->get_x() < p->get_x() + 1))
			{
				const auto y_test = p->get_y() - enemy->get_y();
				if (y_test < enemy_y)
				{
					enemy_x = enemy->get_x();
					enemy_y = y_test;
					shooting_enemy = enemy;
				}
			}
		}
	}

	if (shooting_enemy != nullptr)
	{
		if (p != nullptr)
		{
			const auto probability = static_cast<double>(rand()) / RAND_MAX;
			auto comparison_probability = 0.5;
			if (shooting_enemy->get_x() == p->get_x())
			{
				comparison_probability = 1.0;
			}
			
			if (probability <= comparison_probability)
			{
				if ((enemy_x > p->get_x() - 5) && (enemy_x < p->get_x() + 5))
				{
					auto* eb = new enemy_bullet(enemy_x, enemy_y + 1);
					game_objects.push_back(eb);
				}
			}
		}
		shooting_enemy = nullptr;
	}

	if (drop)
	{
		n_dir_ *= -1;
		for (auto* enemy : enemies_)
		{
			enemy->set_y(enemy->get_y() + 1);
		}
	}

	enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(), [](game_object* g) { return g->get_deleted(); }), enemies_.end());

	update_speed_mod(state);

	if (enemies_.empty())
	{
		state.won = true;
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

void enemy_manager::collided_with(std::vector<game_object*>& game_objects, game_state& state)
{
	for (auto* e : enemies_)
	{
		e->collided_with(game_objects, state);
	}
}