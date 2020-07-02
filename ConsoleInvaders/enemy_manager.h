#pragma once

#include <vector>

#include "game_object.h"

class enemy_manager final : public game_object
{
private:
	const float f_speed_ = 20.0f;
	float f_delta_t_ = 0.0f;
	int n_dir_ = 1;
	
	std::vector<game_object*> enemies_;

	void initialise_enemies();
	
public:
	enemy_manager(int x, int y);

	void update(std::vector<game_object*>& game_objects, keyboard* input, float elapsed) override;
	void draw(console_screen* screen) override;
	void collided_with(std::vector<game_object*>& game_objects) override;
};
