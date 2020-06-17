#pragma once

#include "console_screen.h"
#include "game_timer.h"
#include "keyboard.h"

class game_object
{
	
protected:
	console_screen* p_screen_;
	game_timer* p_timer_;
	keyboard* p_input_;
	
	float x_ = 0.0f;
	float y_ = 0.0f;
	
public:
	game_object(console_screen* screen, game_timer* timer, keyboard* input);
	virtual ~game_object() = default;

	float get_x() const { return x_; }
	float get_y() const { return y_; }
	
	virtual void update() = 0;
	virtual void draw() = 0;
};

