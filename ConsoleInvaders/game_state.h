#pragma once

struct game_state
{
	bool lost = false;
	bool won = false;
	int lives = 3;
	int score = 0;
	int high_score = 0;
	int fps = 0;
	int player_shots = 0;
	int enemy_kill_count = 0;
	float enemy_speed_mod = 2.0f;
};