#include "bunker.h"

bunker::bunker(const int x, const int y) : game_object(x, y)
{
}

void bunker::update(std::vector<game_object*>& game_objects, keyboard* input, float elapsed)
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

void bunker::collided_with(std::vector<game_object*>& game_objects)
{
	// check if enemy or player bullets collided
}