#include "game_timer.h"

game_timer::game_timer()
{
	reset();
}

void game_timer::reset()
{
	elapsed_timer_1_ = std::chrono::system_clock::now();
	elapsed_timer_2_ = std::chrono::system_clock::now();
	frame_timer_ = std::chrono::high_resolution_clock::now();
}

float game_timer::get_elapsed()
{
	elapsed_timer_2_ = std::chrono::system_clock::now();
	const std::chrono::duration<float> elapsed = elapsed_timer_2_ - elapsed_timer_1_;
	elapsed_timer_1_ = elapsed_timer_2_;
	return elapsed.count();
}

void game_timer::frame()
{
	n_frame_count_++;

	if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - frame_timer_) >= std::chrono::seconds{ 1 }) {
		frame_timer_ = std::chrono::high_resolution_clock::now();
		n_fps_ = n_frame_count_;
		n_frame_count_ = 0;
	}
}

int game_timer::get_fps() const
{
	return n_fps_;
}
