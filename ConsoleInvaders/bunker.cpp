#include "bunker.h"
#include "bullet.h"

bunker::bunker(const int x, const int y) : game_object(x, y)
{
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
				screen->plot_char(x_ + x, y_ + y, n_bunker_chr);
			}
		}
	}
}

void bunker::collided_with(std::vector<game_object*>& game_objects, game_state& state)
{
	for(auto* o : game_objects)
	{
		auto* pb = dynamic_cast<bullet*>(o);
		auto* eb = dynamic_cast<bullet*>(o);
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
								shape_[y * n_bunker_width_ + x] = '0';
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