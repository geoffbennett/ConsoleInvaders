#pragma once

#include <iostream>

#include "game_object.h"

class bunker final : public game_object
{
	const short n_100_pc_ = 0x2588;
	const short n_075_pc_ = 0x2593;
	const short n_050_px_ = 0x2592;
	const short n_025_pc_ = 0x2591;
	const short n_000_pc_ = 0x0020;
	
	std::wstring shape_ = L".444.44444444444...4";
	int n_bunker_width_ = 5;
	int n_bunker_height_ = 4;
public:
	bunker(int x, int y);

	void update(std::vector<game_object*>& game_objects, keyboard* input, float elapsed, game_state& state) override;
	void draw(console_screen* screen) override;
	void collided_with(std::vector<game_object*>& game_objects, game_state& state) override;
};