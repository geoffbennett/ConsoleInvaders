#pragma once

#include <vector>

#include "console_screen.h"
#include "keyboard.h"

struct game_state;

class game_object
{
protected:
	bool deleted_ = false;
	int x_ = 0;
	int y_ = 0;
	
public:
	game_object(int x, int y);
	virtual ~game_object() = default;

	int get_x() const { return x_; }
	int get_y() const { return y_; }

	void set_x(const int x) { x_ = x; }
	void set_y(const int y) { y_ = y; }

	bool get_deleted() const { return deleted_; }
	void set_deleted(const bool d) { deleted_ = d; }
	
	virtual void update(std::vector<game_object*>& game_objects, keyboard* input, float elapsed, game_state& state) = 0;
	virtual void draw(console_screen* screen) = 0;
	virtual void collided_with(std::vector<game_object*>& game_objects, game_state& state) = 0;
};

