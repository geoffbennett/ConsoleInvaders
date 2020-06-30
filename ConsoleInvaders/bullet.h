#pragma once

#include "game_object.h"

class bullet : public game_object
{
public:
	void update(keyboard* input, float elapsed) override;
	void draw(console_screen* screen) override;
	void collided_with(std::vector<game_object*>* game_objects) override;
};

