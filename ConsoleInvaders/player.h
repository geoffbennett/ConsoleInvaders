#pragma once

#include "game_object.h"

class player final : public game_object
{
	const float f_speed_ = 40.0f;
	float f_delta_t_ = 0.0f;
	
public:
	player(int x, int y);
	
	void update(std::vector<game_object*>& game_objects, keyboard* input, float elapsed, game_state& state) override;
	void draw(console_screen* screen) override;
	void collided_with(std::vector<game_object*>& game_objects, game_state& state) override;
};

