#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

using namespace std;

#include <Windows.h>

enum class e_mode
{
	mode_intro = 0,
	mode_game = 1,
	mode_win = 2,
	mode_lost = 3
};

struct enemy
{
	int pos_x = 0;
	int pos_y = 0;
	wchar_t chr = 0;
	int points = 0;
	bool remove = false;
};

struct enemy_bullet
{
	int pos_x = 0;
	int pos_y = 0;
	wchar_t chr = 0;
	bool remove = false;
};

struct bunker
{
	int pos_x = 0;
	int pos_y = 0;
	wstring shape;
};

// Screen variables
HANDLE h_screen = nullptr;
wchar_t* screen = nullptr;
wchar_t* buffer = nullptr;

// Global game state
auto n_screen_width = 80;
auto n_screen_height = 30;
auto b_draw_stats = false;
auto b_draw_stats_latch = false;
auto n_fps = 0;
auto n_lives = 3;
auto n_score = 0;
auto n_high_score = 0;
auto b_lost = false;
auto b_won = false;
auto n_mode = e_mode::mode_intro;

// Input state
auto b_key_stats = false;
auto b_key_left = false;
auto b_key_right = false;
auto b_key_shoot = false;
auto b_key_quit = false;
auto b_key_start = false;

// Player
auto n_player_row = n_screen_height - 4;
auto n_player_pos = n_screen_width / 2;
auto b_player_shooting = false;
auto n_player_speed_count = 0;
auto n_player_speed_delta = 200;

auto n_bullet_x = 0;
auto n_bullet_y = 0;
auto n_bullet_speed_count = 0;
auto n_bullet_speed_delta = 50;

const short n_chr_player = 0x2569;
const short n_chr_player_explosion = 0x0023;
const short n_chr_player_bullet = 0x00b7;

auto n_current_player_chr = n_chr_player;

// Enemies
vector<enemy> enemies;
vector<enemy_bullet> enemy_bullets;
auto enemy_dir = 1;
auto n_enemy_speed_count = 0;
auto n_enemy_speed_delta = 1200;
auto n_enemies_killed = 0;
auto n_enemy_speed_mod = 10;
auto n_enemy_speed_adjustment = 150;
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
const auto n_bunker_y = n_screen_height - 10;
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

// === SCREEN ===

void screen_initialise()
{
	screen = new wchar_t[n_screen_width * n_screen_height];
	for (auto i = 0; i < n_screen_width * n_screen_height; i++) screen[i] = L' ';
	h_screen = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);
	SetConsoleActiveScreenBuffer(h_screen);
}

void screen_cleanup()
{
	CloseHandle(h_screen);
}

void screen_refresh()
{
	DWORD dw_bytes_written = 0;
	WriteConsoleOutputCharacter(h_screen, screen, n_screen_width * n_screen_height, { 0, 0 }, &dw_bytes_written);
}

// === BACK BUFFER ===

void buffer_initialise()
{
	buffer = new wchar_t[n_screen_width * n_screen_height];
}

void buffer_clear()
{
	for (auto i = 0; i < n_screen_width * n_screen_height; i++) buffer[i] = L' ';
}

void buffer_present()
{
	for (auto i = 0; i < n_screen_width * n_screen_height; i++) screen[i] = buffer[i];
}

void buffer_plot_char(const int x, const int y, const wchar_t chr)
{
	buffer[y * n_screen_width + x] = chr;
}

void buffer_draw_text(const int x, const int y, const size_t size, const wchar_t* fmt, ...)
{
	va_list my_args;
	va_start(my_args, fmt);
	vswprintf_s(&buffer[y * n_screen_width + x], size, fmt, my_args);
	va_end(my_args);
}

void buffer_draw_text_v(const int x, const int y, const size_t size, const wchar_t* fmt, va_list args)
{
	vswprintf_s(&buffer[y * n_screen_width + x], size, fmt, args);
}

void buffer_draw_text_hc(const int y, const size_t size, const wchar_t* fmt, ...)
{
	va_list my_args;
	va_start(my_args, fmt);
	const auto n_left = static_cast<int>(n_screen_width - size) / 2;
	buffer_draw_text_v(n_left, y, size, fmt, my_args);
	va_end(my_args);
}

// === INPUT ===

void input_sample()
{
	b_key_stats = (0x8000 & GetAsyncKeyState(VK_F1)) != 0;
	b_key_left = (0x8000 & GetAsyncKeyState(VK_LEFT)) != 0;
	b_key_right = (0x8000 & GetAsyncKeyState(VK_RIGHT)) != 0;
	b_key_shoot = (0x8000 & GetAsyncKeyState(VK_SPACE)) != 0;
	b_key_quit = (0x8000 & GetAsyncKeyState(VK_ESCAPE)) != 0;
	b_key_start = (0x8000 & GetAsyncKeyState(VK_RETURN)) != 0;
}

void input_handle_stats_toggle()
{
	if (b_key_stats)
	{
		if (!b_draw_stats_latch && !b_draw_stats)
		{
			b_draw_stats = true;
		}
		else if (!b_draw_stats_latch && b_draw_stats)
		{
			b_draw_stats = false;
		}
		b_draw_stats_latch = true;
	}
	else
	{
		b_draw_stats_latch = false;
	}
}

// === GAME ===

void game_initialise_enemies(const int start_pos)
{
	const auto start_x = 10;
	for (auto r = 0; r < 5; r++)
	{
		for (auto c = 0; c < 11; c++)
		{
			switch (r)
			{
			case 0:
				enemies.push_back({ start_x + c * 4, start_pos + r * 2, L'^', 30 });
				break;
			case 1:
			case 2:
				enemies.push_back({ start_x + c * 4, start_pos + r * 2, L'H', 20 });
				break;
			default:
				enemies.push_back({ start_x + c * 4, start_pos + r * 2, L'M', 10 });
				break;
			}
		}
	}
}

void game_initialise_bunkers()
{
	bunker b1;
	b1.pos_y = n_bunker_y;
	b1.pos_x = n_bunker_1_x;
	b1.shape.append(L".444.");
	b1.shape.append(L"44444");
	b1.shape.append(L"44444");
	b1.shape.append(L"4...4");
	bunkers.push_back(b1);

	bunker b2;
	b2.pos_y = n_bunker_y;
	b2.pos_x = n_bunker_2_x;
	b2.shape.append(L".444.");
	b2.shape.append(L"44444");
	b2.shape.append(L"44444");
	b2.shape.append(L"4...4");
	bunkers.push_back(b2);

	bunker b3;
	b3.pos_y = n_bunker_y;
	b3.pos_x = n_bunker_3_x;
	b3.shape.append(L".444.");
	b3.shape.append(L"44444");
	b3.shape.append(L"44444");
	b3.shape.append(L"4...4");
	bunkers.push_back(b3);

	bunker b4;
	b4.pos_y = n_bunker_y;
	b4.pos_x = n_bunker_4_x;
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
		if (enemy.pos_x == n_bullet_x && enemy.pos_y == n_bullet_y)
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
					if (((bunker.pos_x + x) == n_bullet_x) && ((bunker.pos_y + y) == n_bullet_y))
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
	for (auto bullet : enemy_bullets)
	{
		if ((bullet.pos_x == n_player_pos) && (bullet.pos_y == n_player_row))
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
						if (((bunker.pos_x + x) == bullet.pos_x) && ((bunker.pos_y + y) == bullet.pos_y))
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
	for (auto& enemy : enemies)
	{
		for (auto& bunker : bunkers)
		{
			for (auto x = 0; x < n_bunker_width; x++)
			{
				for (auto y = 0; y < n_bunker_height; y++)
				{
					if (bunker.shape[y * n_bunker_width + x] != L'.')
					{
						if (((bunker.pos_x + x) == enemy.pos_x) && ((bunker.pos_y + y) == enemy.pos_y))
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

void game_process_player()
{
	n_player_speed_count++;
	if (n_player_speed_count <= n_player_speed_delta) return;
	n_player_speed_count = 0;

	if (n_current_player_chr == n_chr_player_explosion)
	{
		n_current_player_chr = n_chr_player;
	}

	if (b_key_left && n_player_pos > 0) n_player_pos--;
	if (b_key_right && n_player_pos < n_screen_width - 1) n_player_pos++;
}

void game_process_bullet()
{
	n_bullet_speed_count++;
	if (n_bullet_speed_count <= n_bullet_speed_delta) return;
	n_bullet_speed_count = 0;

	if (!b_player_shooting && b_key_shoot)
	{
		b_player_shooting = true;
		n_bullet_x = n_player_pos;
		n_bullet_y = n_player_row;
	}

	if (b_player_shooting)
	{
		n_bullet_y--;
		if (n_bullet_y == 1)
		{
			b_player_shooting = false;
		}

		if (game_player_bullet_hit_enemy())
		{
			n_enemies_killed++;
			if (n_enemies_killed % n_enemy_speed_mod == 0)
			{
				n_enemy_speed_delta -= n_enemy_speed_adjustment;
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

void game_enemy_shoot()
{
	n_enemy_shot_wait++;
	const auto rand_val = rand() % 14;
	if ((rand_val == 11) && !b_enemy_shooting && (n_enemy_shot_wait > n_enemy_shot_reset))
	{
		n_enemy_shot_wait = 0;
		b_enemy_shooting = true;
		auto enemy_y = n_player_row;
		enemy shooting_enemy;
		for (const auto enemy : enemies)
		{
			if ((enemy.pos_x > n_player_pos - 5) && (enemy.pos_x < n_player_pos + 5))
			{
				const auto y_test = n_player_row - enemy.pos_y;
				if (y_test < enemy_y)
				{
					enemy_y = y_test;
					shooting_enemy = enemy;
				}
			}
		}
		if ((shooting_enemy.pos_x > 0) && (shooting_enemy.pos_y > 0))
		{
			enemy_bullet b;
			b.pos_x = shooting_enemy.pos_x;
			b.pos_y = shooting_enemy.pos_y + 1;
			b.chr = n_chr_alien_bullet_a;
			enemy_bullets.push_back(b);
		}
	}
}

void game_process_enemy_bullet()
{
	n_enemy_bullet_speed_count++;
	if (n_enemy_bullet_speed_count <= n_enemy_bullet_speed_delta) return;
	n_enemy_bullet_speed_count = 0;

	game_enemy_shoot();

	for (auto& bullet : enemy_bullets)
	{
		bullet.pos_y++;
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

		if (bullet.pos_y == n_player_row)
		{
			bullet.remove = true;
		}
	}

	game_enemy_bullet_remove_dead();
}

void game_enemy_update_sprites()
{
	for (auto& enemy : enemies)
	{
		enemy.pos_x += enemy_dir;

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

void game_process_enemies()
{
	n_enemy_speed_count++;
	if (n_enemy_speed_count <= n_enemy_speed_delta) return;
	n_enemy_speed_count = 0;

	auto b_hit_wall = false;
	for (auto& enemy : enemies)
	{
		const auto new_x_pos = enemy.pos_x + enemy_dir;
		b_hit_wall = (new_x_pos == n_screen_width) || (new_x_pos < 0);
		if (b_hit_wall)
		{
			break;
		}
	}

	if (b_hit_wall)
	{
		for (auto& enemy : enemies)
		{
			enemy.pos_y++;
			if (!b_lost)
			{
				b_lost = (enemy.pos_y == n_player_row);
			}
		}
		enemy_dir *= -1;
		return;
	}

	if (game_enemy_hit_bunker())
	{
		b_lost = true;
	}

	game_enemy_update_sprites();
	game_enemy_remove_dead();
	game_enemy_shoot();
}

void game_draw_player()
{
	buffer_plot_char(n_player_pos, n_player_row, n_current_player_chr);
}

void game_draw_bullet()
{
	if (b_player_shooting)
	{
		buffer_plot_char(n_bullet_x, n_bullet_y, n_chr_player_bullet);
	}
}

void game_draw_enemies()
{
	for (auto enemy : enemies)
	{
		buffer_plot_char(enemy.pos_x, enemy.pos_y, enemy.chr);
	}
}

void game_draw_enemy_bullets()
{
	for (auto bullet : enemy_bullets)
	{
		buffer_plot_char(bullet.pos_x, bullet.pos_y, bullet.chr);
	}
}

void game_draw_bunkers()
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
					buffer_plot_char(bunkers[b].pos_x + x, bunkers[b].pos_y + y, n_bunker_chr);
				}
			}
		}
	}
}

void game_draw_ground()
{
	for (auto i = 0; i < n_screen_width; i++)
	{
		buffer_plot_char(i, n_screen_height - 3, L'_');
	}
}

void game_calc_fps()
{
	static auto old_time = std::chrono::high_resolution_clock::now();
	static int fps;

	fps++;

	if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - old_time) >= std::chrono::seconds{ 1 }) {
		old_time = std::chrono::high_resolution_clock::now();
		n_fps = fps;
		fps = 0;
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

void game_draw_hud()
{
	buffer_draw_text_hc(n_screen_height - 1, std::wcslen(msg_quit) + 1, msg_quit);
	buffer_draw_text(1, 0, std::wcslen(msg_score) + 1, msg_score);
	buffer_draw_text(3, 1, 5, msg_score_fmt, n_score);

	buffer_draw_text_hc(0, std::wcslen(msg_high_score) + 1, msg_high_score);
	buffer_draw_text_hc(1, 5, msg_score_fmt, n_high_score);

	if (b_draw_stats)
	{
		buffer_draw_text(n_screen_width - 6, 0, std::wcslen(msg_key_title) + 1, msg_key_title);
		buffer_draw_text(n_screen_width - 6, 1, 6, msg_key_state, b_key_left, b_key_right, b_key_shoot);
		buffer_draw_text(n_screen_width - 10, 2, 10, msg_fps, n_fps);
		buffer_draw_text(n_screen_width - 10, 3, 10, msg_killed, n_enemies_killed);
		buffer_draw_text(n_screen_width - 12, 4, 12, msg_speed, n_enemy_speed_delta);
	}

	const auto* lives_msg = game_get_lives_message();
	buffer_draw_text(1, n_screen_height - 2, std::wcslen(lives_msg) + 1, lives_msg);
}

// === MODES ===

void mode_intro_screen()
{
	buffer_draw_text_hc((n_screen_height / 2) - 4, std::wcslen(msg_title) + 1, msg_title);
	buffer_draw_text_hc((n_screen_height / 2) - 2, std::wcslen(msg_why) + 1, msg_why);
	buffer_draw_text_hc((n_screen_height / 2) + 4, std::wcslen(msg_start) + 1, msg_start);
	buffer_draw_text_hc(n_screen_height - 1, std::wcslen(msg_quit) + 1, msg_quit);
	if (b_key_start)
	{
		n_mode = e_mode::mode_game;
	}
}

void mode_game_play()
{
	game_process_player();
	game_process_bullet();
	game_process_enemies();
	game_process_enemy_bullet();

	game_draw_player();
	game_draw_bullet();
	game_draw_enemies();
	game_draw_enemy_bullets();
	game_draw_bunkers();
	game_draw_ground();

	game_draw_hud();

	if (b_lost)
	{
		n_mode = e_mode::mode_lost;
	}
}

void mode_win_screen()
{
	buffer_draw_text_hc((n_screen_height / 2) - 4, std::wcslen(msg_won) + 1, msg_won);
	buffer_draw_text_hc((n_screen_height / 2) - 2, std::wcslen(msg_score) + 1, msg_score);
	buffer_draw_text_hc((n_screen_height / 2) - 1, 5, msg_score_fmt, n_score);
	buffer_draw_text_hc(n_screen_height - 1, std::wcslen(msg_quit) + 1, msg_quit);
}

void mode_loss_screen()
{
	buffer_draw_text_hc((n_screen_height / 2) - 4, std::wcslen(msg_lost) + 1, msg_lost);
	buffer_draw_text_hc((n_screen_height / 2) - 2, std::wcslen(msg_score) + 1, msg_score);
	buffer_draw_text_hc((n_screen_height / 2) - 1, 5, msg_score_fmt, n_score);
	buffer_draw_text_hc(n_screen_height - 1, std::wcslen(msg_quit) + 1, msg_quit);
}

int main()
{
	try
	{
		screen_initialise();
		buffer_initialise();

		game_initialise_enemies(5);
		game_initialise_bunkers();

		auto b_quit_game = false;
		while (!b_quit_game)
		{
			buffer_clear();

			input_sample();
			input_handle_stats_toggle();

			switch (n_mode)
			{
			case e_mode::mode_intro:
				mode_intro_screen();
				break;
			case e_mode::mode_game:
				mode_game_play();
				break;
			case e_mode::mode_win:
				mode_win_screen();
				break;
			default:
				mode_loss_screen();
				break;
			}

			buffer_present();
			screen_refresh();

			game_calc_fps();
			//this_thread::sleep_for(50ms);
			b_quit_game = b_key_quit;
		}

		screen_cleanup();
		return EXIT_SUCCESS;
	}
	catch (...)
	{
		screen_cleanup();
		cout << "Exception thrown in main.cpp" << endl;
		system("pause");
		return EXIT_FAILURE;
	}
}