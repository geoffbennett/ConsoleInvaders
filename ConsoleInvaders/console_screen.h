#pragma once

#include <iostream>

#include "Windows.h"

enum colour
{
	fg_black = 0x0000,
	fg_dark_blue = 0x0001,
	fg_dark_green = 0x0002,
	fg_dark_cyan = 0x0003,
	fg_dark_red = 0x0004,
	fg_dark_magenta = 0x0005,
	fg_dark_yellow = 0x0006,
	fg_grey = 0x0007,
	fg_dark_grey = 0x0008,
	fg_blue = 0x0009,
	fg_green = 0x000a,
	fg_cyan = 0x000b,
	fg_red = 0x000c,
	fg_magenta = 0x000d,
	fg_yellow = 0x000e,
	fg_white = 0x000f,
	bg_black = 0x0000,
	bg_dark_blue = 0x0010,
	bg_dark_green = 0x0020,
	bg_dark_cyan = 0x0030,
	bg_dark_red = 0x0040,
	bg_dark_magenta = 0x0050,
	bg_dark_yellow = 0x0060,
	bg_grey = 0x0070,
	bg_dark_grey = 0x0080,
	bg_blue = 0x0090,
	bg_green = 0x00a0,
	bg_cyan = 0x00b0,
	bg_red = 0x00c0,
	bg_magenta = 0x00d0,
	bg_yellow = 0x00e0,
	bg_white = 0x00f0,
};

class console_screen
{
	HANDLE h_console_ = nullptr;
	SMALL_RECT r_console_ = {0, 0, 0, 0};
	CHAR_INFO* p_screen_buffer_ = nullptr;
	
	int n_width_ = 0;
	int n_height_ = 0;

public:
	console_screen(int width, int height, const wchar_t* title);
	~console_screen();

	int get_width() const { return n_width_; }
	int get_height() const { return n_height_; }
	
	void clip(int& x, int& y) const;
	void console_initialise();
	void clear() const;
	void initialise();
	void cleanup() const;
	void present();
	CHAR_INFO* peek(int x, int y) const;
	void draw(int x, int y, short c, short col) const;
	void draw_text(int x, int y, std::wstring c, short col) const;
	void draw_text_centered(int y, const std::wstring& c, short col) const;
};