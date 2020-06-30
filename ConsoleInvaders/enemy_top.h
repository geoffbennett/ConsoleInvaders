#pragma once

#include "game_object.h"

class enemy_top final : public game_object
{
private:
	const short n_chr_alien_1_a_ = 0x005e;
	const short n_chr_alien_1_b_ = 0x002d;
	const short n_chr_alien_explosion_ = 0x002a;

	wchar_t frames_[2] = { L'^', L'-' };
	wchar_t f1_ = L'^';
	wchar_t f2_ = L'-';
	wchar_t cur_ = f1_;
	
public:
	enemy_top(const int x, const int y) : game_object(x, y)
	{
	}

	void draw(console_screen* screen) override;
	void update(keyboard* input, float elapsed) override;
	void collided(game_object* target) override;
};
