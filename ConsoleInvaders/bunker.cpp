#include "bunker.h"
#include "bullet.h"
#include "enemy_bullet.h"

bunker::bunker(const int x, const int y) : game_object(x, y)
{
}

bool bunker::is_hit(const int x, const int y)
{
	for (auto i_x = 0; i_x < n_bunker_width_; i_x++)
	{
		for (auto i_y = 0; i_y < n_bunker_height_; i_y++)
		{
			if (shape_[i_y * n_bunker_width_ + i_x] != L'.')
			{
				if ((x_ + i_x == x) && (y_ + i_y == y))
				{
					return true;
				}
			}
		}
	}
	return false;
}

void bunker::update(std::vector<game_object*>& game_objects, keyboard* input, float elapsed, game_state& state)
{
}

void bunker::draw(console_screen* screen)
{
	for (auto x = 0; x < n_bunker_width_; x++)
	{
		for (auto y = 0; y < n_bunker_height_; y++)
		{
			if (shape_[y * n_bunker_width_ + x] != L'.')
			{
				short n_bunker_chr;
				switch (shape_[y * n_bunker_width_ + x])
				{
				case '4':
					n_bunker_chr = n_100_pc_;
					break;
				case '3':
					n_bunker_chr = n_075_pc_;
					break;
				case '2':
					n_bunker_chr = n_050_px_;
					break;
				case '1':
					n_bunker_chr = n_025_pc_;
					break;
				default:
					n_bunker_chr = n_000_pc_;
					break;
				}
				screen->draw(x_ + x, y_ + y, n_bunker_chr, fg_green);
			}
		}
	}
}

void bunker::collided_with(std::vector<game_object*>& game_objects, game_state& state)
{
	for(auto* o : game_objects)
	{
		auto* pb = dynamic_cast<bullet*>(o);
		auto* eb = dynamic_cast<enemy_bullet*>(o);
		if (pb != nullptr || eb != nullptr)
		{
			const auto bullet_x = o->get_x();
			const auto bullet_y = o->get_y();
			for(auto x = 0; x < n_bunker_width_; x++)
			{
				for(auto y = 0; y < n_bunker_height_; y++)
				{
					if (shape_[y * n_bunker_width_ + x] != L'.')
					{
						if ((x_ + x == bullet_x) && (y_ + y == bullet_y))
						{
							auto b_hit = false;
							switch (shape_[y * n_bunker_width_ + x])
							{
							case '4':
								shape_[y * n_bunker_width_ + x] = '3';
								b_hit = true;
								break;
							case '3':
								shape_[y * n_bunker_width_ + x] = '2';
								b_hit = true;
								break;
							case '2':
								shape_[y * n_bunker_width_ + x] = '1';
								b_hit = true;
								break;
							case '1':
								shape_[y * n_bunker_width_ + x] = '.';
								b_hit = true;
								break;
							default:
								break;
							}
							if (b_hit)
							{
								o->set_deleted(true);
							}
						}
					}
				}
			}
		}
	}
}