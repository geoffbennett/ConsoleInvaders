#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>

#include <Windows.h>

#include "bunker.h"
#include "enemy_manager.h"
#include "game_object.h"
#include "player.h"
#include "console_screen.h"
#include "game_timer.h"
#include "keyboard.h"

using namespace std;

enum class e_mode
{
	intro = 0,
	game = 1,
	win = 2,
	lost = 3
};

struct enemy_bullet
{
	int x = 0;
	int y = 0;
	wchar_t chr = 0;
	bool remove = false;
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
auto n_mode = e_mode::intro;

vector<game_object*> objects;

// Player
auto b_player_shooting = false;

auto n_bullet_x = 0;
auto n_bullet_y = 0;
auto f_bullet_speed = 30.0f;

const short n_chr_player = 0x2569;
const short n_chr_player_explosion = 0x0023;
const short n_chr_player_bullet = 0x00b7;

auto n_current_player_chr = n_chr_player;

// Enemies
vector<enemy_bullet> enemy_bullets;
auto f_enemy_delta_t = 0.0f;
auto f_enemy_speed = 500.0f;
auto n_enemy_dir = 1;
auto n_enemies_killed = 0;
auto n_enemy_speed_increase_threshold = 10;
auto f_enemy_speed_adjustment = 100.0f;
auto b_enemy_shooting = false;
auto n_enemy_shot_wait = 0;
auto n_enemy_shot_reset = 5;

auto n_enemy_bullet_speed_count = 0;
auto n_enemy_bullet_speed_delta = 250;

const short n_chr_alien_bullet_a = 0x007b;
const short n_chr_alien_bullet_b = 0x007d;

// Bunkers
const auto n_bunker_y = n_screen_height - 8;
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
const wchar_t* msg_object_count = L"Objects: %04d";

// === GAME ===

bool game_enemy_bullet_hit_player()
{
	//for (const auto bullet : enemy_bullets)
	//{
	//	if (pos_cmp(bullet.x, f_player_pos) && pos_cmp(bullet.y, f_player_row))
	//	{
	//		n_current_player_chr = n_chr_player_explosion;
	//		return true;
	//	}
	//}
	return false;
}

//bool game_enemy_bullet_hit_bunker()
//{
//	for (auto& bunker : bunkers)
//	{
//		for (auto x = 0; x < n_bunker_width; x++)
//		{
//			for (auto y = 0; y < n_bunker_height; y++)
//			{
//				if (bunker.shape_[y * n_bunker_width + x] != L'.')
//				{
//					for (auto& bullet : enemy_bullets)
//					{
//						if ((bunker.x + x == bullet.x) && (bunker.y + y == bullet.y))
//						{
//							auto b_hit = false;
//							switch (bunker.shape_[y * n_bunker_width + x])
//							{
//							case '4':
//								bunker.shape_[y * n_bunker_width + x] = '3';
//								b_hit = true;
//								break;
//							case '3':
//								bunker.shape_[y * n_bunker_width + x] = '2';
//								b_hit = true;
//								break;
//							case '2':
//								bunker.shape_[y * n_bunker_width + x] = '1';
//								b_hit = true;
//								break;
//							case '1':
//								bunker.shape_[y * n_bunker_width + x] = '0';
//								b_hit = true;
//								break;
//							default:
//								break;
//							}
//							return b_hit;
//						}
//					}
//				}
//			}
//		}
//	}
//	return false;
//}

//bool game_enemy_hit_bunker()
//{
//	for (auto& bunker : bunkers)
//	{
//		for (auto& enemy : enemies)
//		{
//			for (auto x = 0; x < n_bunker_width; x++)
//			{
//				for (auto y = 0; y < n_bunker_height; y++)
//				{
//					if (bunker.shape[y * n_bunker_width + x] != L'.')
//					{
//						if ((bunker.x + x == enemy.x) && (bunker.y + y == enemy.y))
//						{
//							auto b_hit = false;
//							switch (bunker.shape[y * n_bunker_width + x])
//							{
//							case '4':
//								bunker.shape[y * n_bunker_width + x] = '3';
//								b_hit = true;
//								break;
//							case '3':
//								bunker.shape[y * n_bunker_width + x] = '2';
//								b_hit = true;
//								break;
//							case '2':
//								bunker.shape[y * n_bunker_width + x] = '1';
//								b_hit = true;
//								break;
//							case '1':
//								bunker.shape[y * n_bunker_width + x] = '0';
//								b_hit = true;
//								break;
//							default:
//								break;
//							}
//							return b_hit;
//						}
//					}
//				}
//			}
//		}
//	}
//	return false;
//}

void game_process_player(const float elapsed, keyboard* input)
{
	if (n_current_player_chr == n_chr_player_explosion)
	{
		n_current_player_chr = n_chr_player;
	}

	//if (input->get_key(VK_LEFT).held) f_player_pos -= f_player_speed * elapsed;
	//if (input->get_key(VK_RIGHT).held) f_player_pos += f_player_speed * elapsed;
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
	//if ((rand_val == 11) && !b_enemy_shooting && (n_enemy_shot_wait > n_enemy_shot_reset))
	//{
	//	n_enemy_shot_wait = 0;
	//	b_enemy_shooting = true;
	//	auto enemy_y = f_player_row;
	//	enemy shooting_enemy;
	//	for (const auto enemy : enemies)
	//	{
	//		if ((enemy.x > f_player_pos - 5) && (enemy.x < f_player_pos + 5))
	//		{
	//			const auto y_test = f_player_row - enemy.y;
	//			if (y_test < enemy_y)
	//			{
	//				enemy_y = y_test;
	//				shooting_enemy = enemy;
	//			}
	//		}
	//	}
	//	if ((shooting_enemy.x > 0) && (shooting_enemy.y > 0))
	//	{
	//		enemy_bullet b;
	//		b.x = shooting_enemy.x;
	//		b.y = shooting_enemy.y + 1.0f;
	//		b.chr = n_chr_alien_bullet_a;
	//		enemy_bullets.push_back(b);
	//	}
	//}
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

		//if (game_enemy_bullet_hit_bunker())
		//{
		//	bullet.remove = true;
		//}

		//if (pos_cmp(bullet.y, f_player_row))
		//{
		//	bullet.remove = true;
		//}
	}

	game_enemy_bullet_remove_dead();
}

//void game_process_enemies(const float elapsed)
//{
//
//	if (game_enemy_hit_bunker())
//	{
//		b_lost = true;
//	}
//
//	if (b_action_pulse)
//	{
//		game_enemy_shoot(elapsed);
//	}
//}

void game_draw_enemy_bullets(console_screen* screen)
{
	for (const auto bullet : enemy_bullets)
	{
		screen->plot_char(bullet.x, bullet.y, bullet.chr);
	}
}

void game_draw_ground(console_screen* screen)
{
	for (auto i = 0; i < n_screen_width; i++)
	{
		screen->plot_char(i, n_screen_height - 2, L'_');
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
	screen->draw_text_centered(n_screen_height - 1, std::wcslen(msg_quit) + 1, msg_quit);

	wchar_t s[80];
	screen->draw_text(1, 0, std::wcslen(msg_score) + 1, msg_score);
	swprintf_s(s, 5, msg_score_fmt, n_score);
	screen->draw_text(3, 1, std::wcslen(s) + 1, s);

	screen->draw_text_centered(0, std::wcslen(msg_high_score) + 1, msg_high_score);
	swprintf_s(s, 5, msg_score_fmt, n_high_score);
	screen->draw_text_centered(1, 5, s);

	if (b_draw_stats)
	{
		screen->draw_text(n_screen_width - 6, 0, std::wcslen(msg_key_title) + 1, msg_key_title);
		swprintf_s(s, 80, msg_key_state, input->get_key(VK_LEFT).held, input->get_key(VK_RIGHT).held, input->get_key(VK_SPACE).held);
		screen->draw_text(n_screen_width - 6, 1, 6, s);
		swprintf_s(s, 80, msg_fps, fps);
		screen->draw_text(n_screen_width - 10, 2, 10, s);
		swprintf_s(s, 80, msg_killed, n_enemies_killed);
		screen->draw_text(n_screen_width - 10, 3, 10, s);
		swprintf_s(s, 80, msg_speed, f_enemy_speed);
		screen->draw_text(n_screen_width - 12, 4, 12, s);
		swprintf_s(s, 80, msg_object_count, objects.size());
		screen->draw_text(n_screen_width - 14, 5, 14, s);
	}

	const auto* lives_msg = game_get_lives_message();
	screen->draw_text(1, n_screen_height - 1, std::wcslen(lives_msg) + 1, lives_msg);
}

// === MODES ===

void mode_intro_screen(const float elapsed, keyboard* input, console_screen* screen)
{
	screen->draw_text_centered((n_screen_height / 2) - 4, std::wcslen(msg_title) + 1, msg_title);
	screen->draw_text_centered((n_screen_height / 2) - 2, std::wcslen(msg_why) + 1, msg_why);
	screen->draw_text_centered((n_screen_height / 2) + 4, std::wcslen(msg_start) + 1, msg_start);
	screen->draw_text_centered((n_screen_height - 1), std::wcslen(msg_quit) + 1, msg_quit);
	
	if (input->get_key(VK_RETURN).pressed)
	{
		n_mode = e_mode::game;
	}
}

void mode_game_play(const float elapsed, keyboard* input, console_screen* screen, const int fps)
{
	//game_process_enemies(elapsed);
	//game_process_enemy_bullet(elapsed);

	//game_draw_enemy_bullets(screen);
	game_draw_ground(screen);

	objects.erase(std::remove_if(objects.begin(), objects.end(), [](game_object* g) { return g->get_deleted(); }), objects.end());

	for (auto& o : objects)
	{
		o->collided_with(objects);
		o->update(objects, input, elapsed);
		o->draw(screen);
	}

	game_draw_hud(fps, input, screen);

	if (b_lost)
	{
		n_mode = e_mode::lost;
	}
}

void mode_win_screen(const float elapsed, keyboard* input, console_screen* screen)
{
	screen->draw_text_centered((n_screen_height / 2) - 4, std::wcslen(msg_won) + 1, msg_won);
	screen->draw_text_centered((n_screen_height / 2) - 2, std::wcslen(msg_score) + 1, msg_score);

	wchar_t s[80];
	swprintf_s(s, 80, msg_score_fmt, n_score);
	screen->draw_text_centered((n_screen_height / 2) - 1, std::wcslen(s) + 1, s);
	
	screen->draw_text_centered((n_screen_height - 1), std::wcslen(msg_quit) + 1, msg_quit);
}

void mode_loss_screen(const float elapsed, keyboard* input, console_screen* screen)
{
	screen->draw_text_centered((n_screen_height / 2) - 4, std::wcslen(msg_lost) + 1, msg_lost);
	screen->draw_text_centered((n_screen_height / 2) - 2, std::wcslen(msg_score) + 1, msg_score);

	wchar_t s[80];
	swprintf_s(s, 80, msg_score_fmt, n_score);
	screen->draw_text_centered((n_screen_height / 2) - 1, std::wcslen(s) + 1, s);

	screen->draw_text_centered((n_screen_height - 1), std::wcslen(msg_quit) + 1, msg_quit);
}

int main()
{
	try
	{
		auto* const timer = new game_timer();
		auto* const input = new keyboard();
		auto* const screen = new console_screen(80, 30, msg_title);

		auto b_quit_game = false;
		auto b_init_game = false;
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
			case e_mode::intro:
				mode_intro_screen(f_elapsed, input, screen);
				break;
			case e_mode::game:
				if (!b_init_game)
				{
					objects.reserve(1000);
					auto* p = new player(screen->get_width() / 2, screen->get_height() - 3);
					objects.push_back(p);

					auto* em = new enemy_manager(10, 5);
					objects.push_back(em);

					auto* b1 = new bunker(n_bunker_1_x, n_bunker_y);
					objects.push_back(b1);

					auto* b2 = new bunker(n_bunker_2_x, n_bunker_y);
					objects.push_back(b2);

					auto* b3 = new bunker(n_bunker_3_x, n_bunker_y);
					objects.push_back(b3);

					auto* b4 = new bunker(n_bunker_4_x, n_bunker_y);
					objects.push_back(b4);

					b_init_game = true;
				}
				
				mode_game_play(f_elapsed, input, screen, timer->get_fps());
				break;
			case e_mode::win:
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

		for(auto& go : objects)
		{
			delete go;
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