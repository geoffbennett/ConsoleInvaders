#pragma once

#include "game_object.h"

class enemy_bullet final : public game_object
{
	const short n_chr_alien_bullet_a = 0x007b;
	const short n_chr_alien_bullet_b = 0x007d;
	
	const float f_speed_ = 10.0f;
	float f_delta_t_ = 0.0f;

	wchar_t f1_ = L'{';
	wchar_t f2_ = L'}';
	wchar_t cur_ = f1_;

public:
	enemy_bullet(const int x, const int y) : game_object(x, y) {}

	void update(std::vector<game_object*>& game_objects, keyboard* input, float elapsed, game_state& state) override;
	void draw(console_screen* screen) override;
	void collided_with(std::vector<game_object*>& game_objects, game_state& state) override;
};