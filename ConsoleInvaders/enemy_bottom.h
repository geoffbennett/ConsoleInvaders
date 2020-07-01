#pragma once
#include "game_object.h"

class enemy_bottom final : public game_object
{
private:
	const short n_chr_alien_3_a_ = 0x004d;
	const short n_chr_alien_3_b_ = 0x0057;
	const short n_chr_alien_explosion_ = 0x002a;

	wchar_t frames_[2] = { L'M', L'W' };
	wchar_t f1_ = L'M';
	wchar_t f2_ = L'W';
	wchar_t cur_ = f1_;

public:
	enemy_bottom(const int x, const int y) : game_object(x, y)
	{
	}

	void draw(console_screen* screen) override;
	void update(std::vector<game_object*>* game_objects, keyboard* input, float elapsed) override;
	void collided_with(std::vector<game_object*>* game_objects) override;
};
