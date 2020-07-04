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
#include "game_state.h"
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

// Global game state
auto n_screen_width = 80;
auto n_screen_height = 30;
auto b_draw_stats = false;
auto b_draw_stats_latch = false;
auto n_high_score = 0;
auto n_mode = e_mode::intro;
vector<game_object*> objects;

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
const wchar_t* msg_fps = L"FPS:  %4d";
const wchar_t* msg_lives_3 = L"3 \u2569 \u2569 \u2569";
const wchar_t* msg_lives_2 = L"2 \u2569 \u2569";
const wchar_t* msg_lives_1 = L"1 \u2569";
const wchar_t* msg_shots = L"Shots:  % 4d";
const wchar_t* msg_killed = L"Kills:  % 4d";
const wchar_t* msg_speed = L"Speed: %5.02f";
const wchar_t* msg_object_count = L"Objects:  % 4d";

// === GAME ===

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

void game_draw_ground(console_screen* screen)
{
	for (auto i = 0; i < n_screen_width; i++)
	{
		screen->plot_char(i, n_screen_height - 2, L'_');
	}
}

const wchar_t* game_get_lives_message(game_state& state)
{
	switch (state.lives)
	{
	case 3:
		return msg_lives_3;
	case 2:
		return msg_lives_2;
	default:
		return msg_lives_1;
	}
}

void game_draw_hud(game_state& state, keyboard* input, console_screen* screen)
{
	screen->draw_text_centered(n_screen_height - 1, std::wcslen(msg_quit) + 1, msg_quit);

	wchar_t s[80];
	screen->draw_text(1, 0, std::wcslen(msg_score) + 1, msg_score);
	swprintf_s(s, 5, msg_score_fmt, state.score);
	screen->draw_text(3, 1, std::wcslen(s) + 1, s);

	screen->draw_text_centered(0, std::wcslen(msg_high_score) + 1, msg_high_score);
	swprintf_s(s, 5, msg_score_fmt, n_high_score);
	screen->draw_text_centered(1, 5, s);

	if (b_draw_stats)
	{
		screen->draw_text(n_screen_width - 6, 0, std::wcslen(msg_key_title) + 1, msg_key_title);
		swprintf_s(s, 80, msg_key_state, input->get_key(VK_LEFT).held, input->get_key(VK_RIGHT).held, input->get_key(VK_SPACE).held);
		screen->draw_text(n_screen_width - 6, 1, 6, s);
		swprintf_s(s, 80, msg_fps, state.fps);
		screen->draw_text(n_screen_width - 11, 2, 11, s);
		swprintf_s(s, 80, msg_shots, state.player_shots);
		screen->draw_text(n_screen_width - 13, 3, 13, s);
		swprintf_s(s, 80, msg_killed, state.enemy_kill_count);
		screen->draw_text(n_screen_width - 13, 4, 13, s);
		swprintf_s(s, 80, msg_speed, state.enemy_speed_mod);
		screen->draw_text(n_screen_width - 13, 5, 13, s);
		swprintf_s(s, 80, msg_object_count, objects.size());
		screen->draw_text(n_screen_width - 15, 6, 15, s);
	}

	const auto* lives_msg = game_get_lives_message(state);
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

void mode_game_play(const float elapsed, keyboard* input, console_screen* screen, game_state& state)
{
	//game_process_enemies(elapsed);
	//game_process_enemy_bullet(elapsed);

	//game_draw_enemy_bullets(screen);
	game_draw_ground(screen);

	objects.erase(std::remove_if(objects.begin(), objects.end(), [](game_object* g) { return g->get_deleted(); }), objects.end());

	for (auto& o : objects)
	{
		o->collided_with(objects, state);
		o->update(objects, input, elapsed, state);
		o->draw(screen);
	}

	game_draw_hud(state, input, screen);

	if (state.lost)
	{
		n_mode = e_mode::lost;
	}
}

void mode_win_screen(const float elapsed, keyboard* input, console_screen* screen, game_state& state)
{
	screen->draw_text_centered((n_screen_height / 2) - 4, std::wcslen(msg_won) + 1, msg_won);
	screen->draw_text_centered((n_screen_height / 2) - 2, std::wcslen(msg_score) + 1, msg_score);

	wchar_t s[80];
	swprintf_s(s, 80, msg_score_fmt, state.score);
	screen->draw_text_centered((n_screen_height / 2) - 1, std::wcslen(s) + 1, s);
	
	screen->draw_text_centered((n_screen_height - 1), std::wcslen(msg_quit) + 1, msg_quit);
}

void mode_loss_screen(const float elapsed, keyboard* input, console_screen* screen, game_state& state)
{
	screen->draw_text_centered((n_screen_height / 2) - 4, std::wcslen(msg_lost) + 1, msg_lost);
	screen->draw_text_centered((n_screen_height / 2) - 2, std::wcslen(msg_score) + 1, msg_score);

	wchar_t s[80];
	swprintf_s(s, 80, msg_score_fmt, state.score);
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
		game_state state;

		auto b_quit_game = false;
		auto b_init_game = false;
		
		while (!b_quit_game)
		{
			const auto f_elapsed = timer->get_elapsed();
			input->sample();
			screen->clear();

			state.fps = timer->get_fps();
			
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
				
				mode_game_play(f_elapsed, input, screen, state);
				
				if (state.won) n_mode = e_mode::win;
				if (state.lost) n_mode = e_mode::lost;
				
				break;
			case e_mode::win:
				mode_win_screen(f_elapsed, input, screen, state);
				break;
			default:
				mode_loss_screen(f_elapsed, input, screen, state);
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