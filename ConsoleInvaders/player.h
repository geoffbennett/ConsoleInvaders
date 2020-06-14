#pragma once

#include "game_object.h"

class player : public game_object
{
	float speed_ = 0.0f;
public:
	void update(game_timer* timer, keyboard* input) override;
	void draw(wchar_t* buffer) override;
};

