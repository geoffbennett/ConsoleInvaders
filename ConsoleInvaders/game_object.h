#pragma once

#include "console_screen.h"
#include "keyboard.h"

class game_object
{
	
protected:
	int x_ = 0;
	int y_ = 0;
	
public:
	game_object(int x, int y);
	virtual ~game_object() = default;

	int get_x() const { return x_; }
	int get_y() const { return y_; }

	void set_x(const int x) { x_ = x; }
	void set_y(const int y) { y_ = y; }
	
	virtual void update(keyboard* input, float elapsed) = 0;
	virtual void draw(console_screen* screen) = 0;
	virtual void collided(game_object* target) = 0;
};

