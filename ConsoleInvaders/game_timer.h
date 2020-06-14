#pragma once

#include <chrono>

class game_timer
{
	std::chrono::time_point<std::chrono::system_clock> elapsed_timer_1_;
	std::chrono::time_point<std::chrono::system_clock> elapsed_timer_2_;

	std::chrono::time_point<std::chrono::high_resolution_clock> frame_timer_;
	int n_frame_count_ = 0;
	int n_fps_ = 0;

public:
	game_timer();

	void reset();
	float get_elapsed();

	void frame();
	int get_fps() const;
};

