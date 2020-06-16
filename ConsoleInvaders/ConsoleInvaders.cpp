#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>

using namespace std;

#include <Windows.h>

#include "console_screen.h"
#include "game_timer.h"
#include "keyboard.h"

enum class e_mode
{
	mode_intro = 0,
	mode_game = 1,
	mode_win = 2,
	mode_lost = 3
};

struct enemy
{
	float x = 0.0f;
	float y = 0.0f;
	wchar_t chr = 0;
	int points = 0;
	bool remove = false;
};

struct enemy_bullet
{
	float x = 0;
	float y = 0;
	wchar_t chr = 0;
	bool remove = false;
};

struct bunker
{
	float x = 0.0f;
	float y = 0.0f;
	wstring shape;
};

// Global game state
auto n_screen_width = 80;
auto n_screen_height = 30;
auto b_draw_stats = false;
auto b_draw_stats_latch = false;
auto n_lives = 3;
auto n_score = 0;
auto n_high_score = 0;
auto b_lost = false;
auto b_won = false;
auto n_mode = e_mode::mode_intro;

// Player
auto f_player_speed = 30.0f;
auto f_player_row = static_cast<float>(n_screen_height - 4.0f);
auto f_player_pos = static_cast<float>(n_screen_width / 2.0f);
auto b_player_shooting = false;

auto f_bullet_x = 0.0f;
auto f_bullet_y = 0.0f;
auto f_bullet_speed = 30.0f;

const short n_chr_player = 0x2569;
const short n_chr_player_explosion = 0x0023;
const short n_chr_player_bullet = 0x00b7;

auto n_current_player_chr = n_chr_player;

// Enemies
vector<enemy> enemies;
vector<enemy_bullet> enemy_bullets;
auto f_enemy_speed = 2.0f;
auto n_enemies_killed = 0;
auto n_enemy_speed_increase_threshold = 10;
auto n_enemy_speed_adjustment = 3.0f;
auto b_enemy_shooting = false;
auto n_enemy_shot_wait = 0;
auto n_enemy_shot_reset = 5;

auto n_enemy_bullet_speed_count = 0;
auto n_enemy_bullet_speed_delta = 250;

const short n_chr_alien_1_a = 0x005e;
const short n_chr_alien_1_b = 0x002d;
const short n_chr_alien_2_a = 0x0048;
const short n_chr_alien_2_b = 0x0058;
const short n_chr_alien_3_a = 0x004d;
const short n_chr_alien_3_b = 0x0057;
const short n_chr_alien_explosion = 0x002a;
const short n_chr_alien_bullet_a = 0x007b;
const short n_chr_alien_bullet_b = 0x007d;

// Bunkers
const short n_100_pc = 0x2588;
const short n_075_pc = 0x2593;
const short n_050_px = 0x2592;
const short n_025_pc = 0x2591;
const short n_000_pc = 0x0020;
vector<bunker> bunkers;
const auto n_bunker_width = 5;
const auto n_bunker_height = 4;
const auto f_bunker_y = static_cast<float>(n_screen_height) - 10.0f;
const auto n_bunker_1_x = 15;
const auto n_bunker_2_x = 30;
const auto n_bunker_3_x = 45;
const auto n_bunker_4_x = 60;

// Messages
const wchar_t* msg_title = L"C O N S O L E  I N V A D E R S";
const wchar_t* msg_why = L"Why? Because. That's why.";
const wchar_t* msg_start = L"Press ENTER to start";
const wchar_t* msg_quit = L"Press ESC to quit";
const wchar_t* msg_won = L"You saved the planet!";
const wchar_t* msg_lost = L"Everybody died because of you!";
const wchar_t* msg_high_score = L"HI-SCORE";
const wchar_t* msg_score = L"SCORE<1>";
const wchar_t* msg_score_fmt = L"%04d";
const wchar_t* msg_key_title = L"L R S";
const wchar_t* msg_key_state = L"%1d %1d %1d";
const wchar_t* msg_fps = L"FPS: %4d";
const wchar_t* msg_lives_3 = L"3 \u2569 \u2569 \u2569";
const wchar_t* msg_lives_2 = L"2 \u2569 \u2569";
const wchar_t* msg_lives_1 = L"1 \u2569";
const wchar_t* msg_killed = L"Kills: %02d";
const wchar_t* msg_speed = L"Speed: %04d";

// === GAME ===

bool pos_cmp(const float f1, const float f2)
{
	// we don't deal with negative screen space, so we should be
	// able to use math to compare two floats to save the cost of
	// casting to an integer, but I don't know if that would be
	// faster. Need to experiment.
	return static_cast<int>(f1) == static_cast<int>(f2);
}

void game_initialise_enemies(const float start_pos)
{
	const auto start_x = 10.0f;
	for (auto r = 0; r < 5; r++)
	{
		for (auto c = 0; c < 11; c++)
		{
			switch (r)
			{
			case 0:
				enemies.push_back({ start_x + static_cast<float>(c) * 4, start_pos + static_cast<float>(r) * 2, L'^', 30 });
				break;
			case 1:
			case 2:
				enemies.push_back({ start_x + static_cast<float>(c) * 4, start_pos + static_cast<float>(r) * 2, L'H', 20 });
				break;
			default:
				enemies.push_back({ start_x + static_cast<float>(c) * 4, start_pos + static_cast<float>(r) * 2, L'M', 10 });
				break;
			}
		}
	}
}

void game_initialise_bunkers()
{
	bunker b1;
	b1.y = f_bunker_y;
	b1.x = n_bunker_1_x;
	b1.shape.append(L".444.");
	b1.shape.append(L"44444");
	b1.shape.append(L"44444");
	b1.shape.append(L"4...4");
	bunkers.push_back(b1);

	bunker b2;
	b2.y = f_bunker_y;
	b2.x = n_bunker_2_x;
	b2.shape.append(L".444.");
	b2.shape.append(L"44444");
	b2.shape.append(L"44444");
	b2.shape.append(L"4...4");
	bunkers.push_back(b2);

	bunker b3;
	b3.y = f_bunker_y;
	b3.x = n_bunker_3_x;
	b3.shape.append(L".444.");
	b3.shape.append(L"44444");
	b3.shape.append(L"44444");
	b3.shape.append(L"4...4");
	bunkers.push_back(b3);

	bunker b4;
	b4.y = f_bunker_y;
	b4.x = n_bunker_4_x;
	b4.shape.append(L".444.");
	b4.shape.append(L"44444");
	b4.shape.append(L"44444");
	b4.shape.append(L"4...4");
	bunkers.push_back(b4);
}

bool game_player_bullet_hit_enemy()
{
	for (auto& enemy : enemies)
	{
		if (pos_cmp(enemy.x, f_bullet_x) && pos_cmp(enemy.y, f_bullet_y))
		{
			n_score += enemy.points;
			enemy.chr = n_chr_alien_explosion;
			return true;
		}
	}
	return false;
}

bool game_player_bullet_hit_bunker()
{
	for (auto& bunker : bunkers)
	{
		for (auto x = 0; x < n_bunker_width; x++)
		{
			for (auto y = 0; y < n_bunker_height; y++)
			{
				if (bunker.shape[y * n_bunker_width + x] != L'.')
				{
					if (pos_cmp(bunker.x + x, f_bullet_x) && pos_cmp(bunker.y + y, f_bullet_y))
					{
						auto b_hit = false;
						switch (bunker.shape[y * n_bunker_width + x])
						{
						case '4':
							bunker.shape[y * n_bunker_width + x] = '3';
							b_hit = true;
							break;
						case '3':
							bunker.shape[y * n_bunker_width + x] = '2';
							b_hit = true;
							break;
						case '2':
							bunker.shape[y * n_bunker_width + x] = '1';
							b_hit = true;
							break;
						case '1':
							bunker.shape[y * n_bunker_width + x] = '0';
							b_hit = true;
							break;
						default:
							break;
						}
						return b_hit;
					}
				}
			}
		}
	}
	return false;
}

bool game_enemy_bullet_hit_player()
{
	for (const auto bullet : enemy_bullets)
	{
		if (pos_cmp(bullet.x, f_player_pos) && pos_cmp(bullet.y, f_player_row))
		{
			n_current_player_chr = n_chr_player_explosion;
			return true;
		}
	}
	return false;
}

bool game_enemy_bullet_hit_bunker()
{
	for (auto& bunker : bunkers)
	{
		for (auto x = 0; x < n_bunker_width; x++)
		{
			for (auto y = 0; y < n_bunker_height; y++)
			{
				if (bunker.shape[y * n_bunker_width + x] != L'.')
				{
					for (auto& bullet : enemy_bullets)
					{
						if (pos_cmp(bunker.x + x, bullet.x) && pos_cmp(bunker.y + y, bullet.y))
						{
							auto b_hit = false;
							switch (bunker.shape[y * n_bunker_width + x])
							{
							case '4':
								bunker.shape[y * n_bunker_width + x] = '3';
								b_hit = true;
								break;
							case '3':
								bunker.shape[y * n_bunker_width + x] = '2';
								b_hit = true;
								break;
							case '2':
								bunker.shape[y * n_bunker_width + x] = '1';
								b_hit = true;
								break;
							case '1':
								bunker.shape[y * n_bunker_width + x] = '0';
								b_hit = true;
								break;
							default:
								break;
							}
							return b_hit;
						}
					}
				}
			}
		}
	}
	return false;
}

bool game_enemy_hit_bunker()
{
	for (auto& bunker : bunkers)
	{
		for (auto& enemy : enemies)
		{
			for (auto x = 0; x < n_bunker_width; x++)
			{
				for (auto y = 0; y < n_bunker_height; y++)
				{
					if (bunker.shape[y * n_bunker_width + x] != L'.')
					{
						if (pos_cmp(bunker.x + x, enemy.x) && pos_cmp(bunker.y + y, enemy.y))
						{
							auto b_hit = false;
							switch (bunker.shape[y * n_bunker_width + x])
							{
							case '4':
								bunker.shape[y * n_bunker_width + x] = '3';
								b_hit = true;
								break;
							case '3':
								bunker.shape[y * n_bunker_width + x] = '2';
								b_hit = true;
								break;
							case '2':
								bunker.shape[y * n_bunker_width + x] = '1';
								b_hit = true;
								break;
							case '1':
								bunker.shape[y * n_bunker_width + x] = '0';
								b_hit = true;
								break;
							default:
								break;
							}
							return b_hit;
						}
					}
				}
			}
		}
	}
	return false;
}

void game_process_player(const float elapsed, keyboard* input)
{
	if (n_current_player_chr == n_chr_player_explosion)
	{
		n_current_player_chr = n_chr_player;
	}

	if (input->get_key(VK_LEFT).held) f_player_pos -= f_player_speed * elapsed;
	if (input->get_key(VK_RIGHT).held) f_player_pos += f_player_speed * elapsed;
}

void game_process_bullet(const float elapsed, keyboard* input)
{
	if (!b_player_shooting && input->get_key(VK_SPACE).held)
	{
		b_player_shooting = true;
		f_bullet_x = f_player_pos;
		f_bullet_y = f_player_row;
	}

	if (b_player_shooting)
	{
		f_bullet_y -= f_bullet_speed * elapsed;
		if (f_bullet_y <= 1)
		{
			b_player_shooting = false;
		}

		if (game_player_bullet_hit_enemy())
		{
			n_enemies_killed++;
			if (n_enemies_killed % n_enemy_speed_increase_threshold == 0)
			{
				f_enemy_speed += n_enemy_speed_adjustment;
			}
			b_player_shooting = false;
		}

		if (game_player_bullet_hit_bunker())
		{
			b_player_shooting = false;
		}
	}
}

void game_enemy_bullet_remove_dead()
{
	enemy_bullets.erase(std::remove_if(enemy_bullets.begin(), enemy_bullets.end(), [](enemy_bullet& b) { return b.remove; }), enemy_bullets.end());
	b_enemy_shooting = false;
}

void game_enemy_shoot(float elapsed)
{
	n_enemy_shot_wait++;
	const auto rand_val = rand() % 14;
	if ((rand_val == 11) && !b_enemy_shooting && (n_enemy_shot_wait > n_enemy_shot_reset))
	{
		n_enemy_shot_wait = 0;
		b_enemy_shooting = true;
		auto enemy_y = f_player_row;
		enemy shooting_enemy;
		for (const auto enemy : enemies)
		{
			if ((enemy.x > f_player_pos - 5) && (enemy.x < f_player_pos + 5))
			{
				const auto y_test = f_player_row - enemy.y;
				if (y_test < enemy_y)
				{
					enemy_y = y_test;
					shooting_enemy = enemy;
				}
			}
		}
		if ((shooting_enemy.x > 0) && (shooting_enemy.y > 0))
		{
			enemy_bullet b;
			b.x = shooting_enemy.x;
			b.y = shooting_enemy.y + 1.0f;
			b.chr = n_chr_alien_bullet_a;
			enemy_bullets.push_back(b);
		}
	}
}

void game_process_enemy_bullet(const float elapsed)
{
	n_enemy_bullet_speed_count++;
	if (n_enemy_bullet_speed_count <= n_enemy_bullet_speed_delta) return;
	n_enemy_bullet_speed_count = 0;

	//game_enemy_shoot();

	for (auto& bullet : enemy_bullets)
	{
		bullet.y++;
		if (bullet.chr == n_chr_alien_bullet_a)
		{
			bullet.chr = n_chr_alien_bullet_b;
		}
		else if (bullet.chr == n_chr_alien_bullet_b)
		{
			bullet.chr = n_chr_alien_bullet_a;
		}

		if (game_enemy_bullet_hit_player())
		{
			bullet.remove = true;
			n_lives--;
			if (n_lives == 0)
			{
				b_lost = true;
			}
		}

		if (game_enemy_bullet_hit_bunker())
		{
			bullet.remove = true;
		}

		if (pos_cmp(bullet.y, f_player_row))
		{
			bullet.remove = true;
		}
	}

	game_enemy_bullet_remove_dead();
}

void game_enemy_update_sprites(float elapsed)
{
	for (auto& enemy : enemies)
	{
		if (enemy.chr == n_chr_alien_3_a)
		{
			enemy.chr = n_chr_alien_3_b;
		}
		else if (enemy.chr == n_chr_alien_3_b)
		{
			enemy.chr = n_chr_alien_3_a;
		}

		if (enemy.chr == n_chr_alien_2_a)
		{
			enemy.chr = n_chr_alien_2_b;
		}
		else if (enemy.chr == n_chr_alien_2_b)
		{
			enemy.chr = n_chr_alien_2_a;
		}

		if (enemy.chr == n_chr_alien_1_a)
		{
			enemy.chr = n_chr_alien_1_b;
		}
		else if (enemy.chr == n_chr_alien_1_b)
		{
			enemy.chr = n_chr_alien_1_a;
		}

		if (enemy.chr == n_chr_alien_explosion)
		{
			enemy.remove = true;
		}
	}
}

void game_enemy_remove_dead()
{
	enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](enemy& e) { return e.remove; }), enemies.end());
	if (enemies.empty())
	{
		n_mode = e_mode::mode_win;
	}
}

void game_process_enemies(const float elapsed)
{
	auto b_hit_wall = false;
	auto b_action_pulse = false;
	for (auto& enemy : enemies)
	{
		const auto old_x = static_cast<int>(enemy.x);
		enemy.x += f_enemy_speed * elapsed;
		b_action_pulse = old_x != static_cast<int>(enemy.x);

		if (enemy.x >= static_cast<float>(n_screen_width))
		{
			b_hit_wall = true;
			enemy.x = static_cast<float>(n_screen_width);
		}
		else if (enemy.x <= 0.0f)
		{
			b_hit_wall = true;
			enemy.x = 0.0f;
		}
	}

	if (b_hit_wall)
	{
		for (auto& enemy : enemies)
		{
			enemy.y++;
			if (!b_lost)
			{
				b_lost = (enemy.y >= f_player_row);
			}
		}

		f_enemy_speed *= -1;
	}

	if (game_enemy_hit_bunker())
	{
		b_lost = true;
	}

	if (b_action_pulse)
	{
		game_enemy_update_sprites(elapsed);
		game_enemy_remove_dead();
		game_enemy_shoot(elapsed);
	}
}

void game_draw_player(console_screen* screen)
{
	screen->plot_char(f_player_pos, f_player_row, n_current_player_chr);
}

void game_draw_bullet(console_screen* screen)
{
	if (b_player_shooting)
	{
		screen->plot_char(f_bullet_x, f_bullet_y, n_chr_player_bullet);
	}
}

void game_draw_enemies(console_screen* screen)
{
	for (const auto enemy : enemies)
	{
		screen->plot_char(enemy.x, enemy.y, enemy.chr);
	}
}

void game_draw_enemy_bullets(console_screen* screen)
{
	for (const auto bullet : enemy_bullets)
	{
		screen->plot_char(bullet.x, bullet.y, bullet.chr);
	}
}

void game_draw_bunkers(console_screen* screen)
{
	for (auto b = 0; b < 4; b++)
	{
		for (auto x = 0; x < n_bunker_width; x++)
		{
			for (auto y = 0; y < n_bunker_height; y++)
			{
				if (bunkers[b].shape[y * n_bunker_width + x] != L'.')
				{
					short n_bunker_chr;
					switch (bunkers[b].shape[y * n_bunker_width + x])
					{
					case '4':
						n_bunker_chr = n_100_pc;
						break;
					case '3':
						n_bunker_chr = n_075_pc;
						break;
					case '2':
						n_bunker_chr = n_050_px;
						break;
					case '1':
						n_bunker_chr = n_025_pc;
						break;
					default:
						n_bunker_chr = n_000_pc;
						break;
					}
					screen->plot_char(bunkers[b].x + x, bunkers[b].y + y, n_bunker_chr);
				}
			}
		}
	}
}

void game_draw_ground(console_screen* screen)
{
	for (auto i = 0; i < n_screen_width; i++)
	{
		screen->plot_char(static_cast<float>(i), static_cast<float>(n_screen_height) - 3, L'_');
	}
}

const wchar_t* game_get_lives_message()
{
	switch (n_lives)
	{
	case 3:
		return msg_lives_3;
	case 2:
		return msg_lives_2;
	default:
		return msg_lives_1;
	}
}

void game_draw_hud(const int fps, keyboard* input, console_screen* screen)
{
	screen->draw_text_centered(static_cast<float>(n_screen_height) - 1, std::wcslen(msg_quit) + 1, msg_quit);

	wchar_t s[80];
	screen->draw_text(1, 0, std::wcslen(msg_score) + 1, msg_score);
	swprintf_s(s, 5, msg_score_fmt, n_score);
	screen->draw_text(3, 1, std::wcslen(s) + 1, s);

	screen->draw_text_centered(0, std::wcslen(msg_high_score) + 1, msg_high_score);
	swprintf_s(s, 5, msg_score_fmt, n_high_score);
	screen->draw_text_centered(1, 5, s);

	if (b_draw_stats)
	{
		screen->draw_text(static_cast<float>(n_screen_width) - 6, 0, std::wcslen(msg_key_title) + 1, msg_key_title);
		swprintf_s(s, 80, msg_key_state, input->get_key(VK_LEFT).held, input->get_key(VK_RIGHT).held, input->get_key(VK_SPACE).held);
		screen->draw_text(static_cast<float>(n_screen_width) - 6, 1, 6, s);
		swprintf_s(s, 80, msg_fps, fps);
		screen->draw_text(static_cast<float>(n_screen_width) - 10, 2, 10, s);
		swprintf_s(s, 80, msg_killed, n_enemies_killed);
		screen->draw_text(static_cast<float>(n_screen_width) - 10, 3, 10, s);
		swprintf_s(s, 80, msg_speed, f_enemy_speed);
		screen->draw_text(static_cast<float>(n_screen_width) - 12, 4, 12, s);
	}

	const auto* lives_msg = game_get_lives_message();
	screen->draw_text(1, static_cast<float>(n_screen_height) - 2, std::wcslen(lives_msg) + 1, lives_msg);
}

// === MODES ===

void mode_intro_screen(const float elapsed, keyboard* input, console_screen* screen)
{
	screen->draw_text_centered((static_cast<float>(n_screen_height) / 2) - 4, std::wcslen(msg_title) + 1, msg_title);
	screen->draw_text_centered((static_cast<float>(n_screen_height) / 2) - 2, std::wcslen(msg_why) + 1, msg_why);
	screen->draw_text_centered((static_cast<float>(n_screen_height) / 2) + 4, std::wcslen(msg_start) + 1, msg_start);
	screen->draw_text_centered((static_cast<float>(n_screen_height) - 1), std::wcslen(msg_quit) + 1, msg_quit);
	
	if (input->get_key(VK_RETURN).pressed)
	{
		n_mode = e_mode::mode_game;
	}
}

void mode_game_play(const float elapsed, keyboard* input, console_screen* screen, const int fps)
{
	game_process_player(elapsed, input);
	game_process_bullet(elapsed, input);
	game_process_enemies(elapsed);
	game_process_enemy_bullet(elapsed);

	game_draw_bunkers(screen);
	game_draw_player(screen);
	game_draw_bullet(screen);
	game_draw_enemies(screen);
	game_draw_enemy_bullets(screen);
	game_draw_ground(screen);

	game_draw_hud(fps, input, screen);

	if (b_lost)
	{
		n_mode = e_mode::mode_lost;
	}
}

void mode_win_screen(const float elapsed, keyboard* input, console_screen* screen)
{
	screen->draw_text_centered((static_cast<float>(n_screen_height) / 2) - 4, std::wcslen(msg_won) + 1, msg_won);
	screen->draw_text_centered((static_cast<float>(n_screen_height) / 2) - 2, std::wcslen(msg_score) + 1, msg_score);

	wchar_t s[80];
	swprintf_s(s, 80, msg_score_fmt, n_score);
	screen->draw_text_centered((static_cast<float>(n_screen_height) / 2) - 1, std::wcslen(s) + 1, s);
	
	screen->draw_text_centered((static_cast<float>(n_screen_height) - 1), std::wcslen(msg_quit) + 1, msg_quit);
}

void mode_loss_screen(const float elapsed, keyboard* input, console_screen* screen)
{
	screen->draw_text_centered((static_cast<float>(n_screen_height) / 2) - 4, std::wcslen(msg_lost) + 1, msg_lost);
	screen->draw_text_centered((static_cast<float>(n_screen_height) / 2) - 2, std::wcslen(msg_score) + 1, msg_score);

	wchar_t s[80];
	swprintf_s(s, 80, msg_score_fmt, n_score);
	screen->draw_text_centered((static_cast<float>(n_screen_height) / 2) - 1, std::wcslen(s) + 1, s);

	screen->draw_text_centered((static_cast<float>(n_screen_height) - 1), std::wcslen(msg_quit) + 1, msg_quit);
}

int main()
{
	try
	{
		auto* const timer = new game_timer();
		auto* const input = new keyboard();
		auto* const screen = new console_screen(80, 30, msg_title);

		game_initialise_enemies(5);
		game_initialise_bunkers();

		auto b_quit_game = false;
		while (!b_quit_game)
		{
			const auto f_elapsed = timer->get_elapsed();
			input->sample();
			screen->clear();

			if (input->get_key(VK_F1).released)
			{
				b_draw_stats = !b_draw_stats;
			}

			switch (n_mode)
			{
			case e_mode::mode_intro:
				mode_intro_screen(f_elapsed, input, screen);
				break;
			case e_mode::mode_game:
				mode_game_play(f_elapsed, input, screen, timer->get_fps());
				break;
			case e_mode::mode_win:
				mode_win_screen(f_elapsed, input, screen);
				break;
			default:
				mode_loss_screen(f_elapsed, input, screen);
				break;
			}

			screen->flip();
			screen->present();

			timer->frame();

			b_quit_game = input->get_key(VK_ESCAPE).pressed;
		}

		// ERROR: Why does this cause a heap corruption?
		// delete input;
		delete timer;
		delete screen;
		
		return EXIT_SUCCESS;
	}
	catch (...)
	{
		cout << "Exception thrown in main.cpp" << endl;
		system("pause");
		return EXIT_FAILURE;
	}
}