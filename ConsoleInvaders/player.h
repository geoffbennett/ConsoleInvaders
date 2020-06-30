#pragma once

#include "game_object.h"
#include "console_screen.h"

class player final : public game_object
{
	const float f_speed_ = 40.0f;
	float f_delta_t_ = 0.0f;	
	
public:
	player(int x, int y);
	
	void update(keyboard* input, float elapsed) override;
	void draw(console_screen* screen) override;
	void collided(game_object* target) override;
};

