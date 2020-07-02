#pragma once
#include "game_object.h"

class enemy_middle final : public game_object
{
private:
	const short n_chr_alien_2_a_ = 0x0048;
	const short n_chr_alien_2_b_ = 0x0058;
	const short n_chr_alien_explosion_ = 0x002a;

	wchar_t frames_[2] = { L'H', L'X' };
	wchar_t f1_ = L'H';
	wchar_t f2_ = L'X';
	wchar_t dying_ = L'*';
	wchar_t cur_ = f1_;

public:
	enemy_middle(const int x, const int y) : game_object(x, y)
	{
	}

	void draw(console_screen* screen) override;
	void update(std::vector<game_object*>& game_objects, keyboard* input, float elapsed) override;
	void collided_with(std::vector<game_object*>& game_objects) override;
};
