#include "player.h"
#include "bullet.h"
#include "enemy_bullet.h"
#include "game_state.h"

player::player(const int x, const int y) : game_object(x, y)
{
}

void player::update(std::vector<game_object*>& game_objects, keyboard* input, const float elapsed, game_state& state)
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
			state.player_shots++;
			auto* b = new bullet(x_, y_ - 1);
			game_objects.push_back(b);
		}
	}

	if (n_current_player_chr_ == n_chr_player_explosion_)
	{
		n_current_player_chr_ = n_chr_player_;
	}
	
	f_delta_t_ -= 1.0f;
}

void player::draw(console_screen* screen)
{
	screen->draw(x_, y_, n_current_player_chr_, fg_green);
}

void player::collided_with(std::vector<game_object*>& game_objects, game_state& state)
{
	for (auto* o : game_objects)
	{
		auto* eb = dynamic_cast<enemy_bullet*>(o);
		if (eb != nullptr)
		{
			const auto bullet_x = o->get_x();
			const auto bullet_y = o->get_y();

			if ((x_ == bullet_x) && (y_ == bullet_y))
			{
				eb->set_deleted(true);
				n_current_player_chr_ = n_chr_player_explosion_;
				state.lives--;
				if (state.lives == 0)
				{
					state.lost = true;
				}
			}
		}
	}
}