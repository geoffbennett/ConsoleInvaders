#pragma once

#include "game_timer.h"
#include "keyboard.h"

class game_object
{
protected:
	float x_ = 0.0f;
	float y_ = 0.0f;
	
public:
	virtual ~game_object() = default;

	float get_x() const { return x_; }
	float get_y() const { return y_; }
	
	virtual void update(game_timer* timer, keyboard* input) = 0;
	virtual void draw(wchar_t* buffer) = 0;
};

