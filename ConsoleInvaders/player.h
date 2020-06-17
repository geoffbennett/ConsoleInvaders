#pragma once

#include "game_object.h"
#include "console_screen.h"

class player final : public game_object
{
	float f_speed_ = 40.0f;
	
public:
	player(console_screen* screen, game_timer* timer, keyboard* input);
	
	void update() override;
	void draw() override;
};

