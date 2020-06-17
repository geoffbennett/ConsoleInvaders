#pragma once

#include "Windows.h"

class console_screen
{
	HANDLE h_screen_ = nullptr;
	wchar_t* p_screen_ = nullptr;
	wchar_t* p_buffer_ = nullptr;

	int n_width_ = 0;
	int n_height_ = 0;
	const wchar_t* s_title_;

	void construct_screen();
	void construct_buffer();

	void clip(float& x, float& y) const;

public:
	console_screen(int width, int height, const wchar_t* title);
	~console_screen();

	int get_width() const;
	int get_height() const;
	
	void clear() const;
	void flip() const;
	void present() const;

	void plot_char(float x, float y, wchar_t chr) const;
	void draw_text(float x, float y, size_t size, const wchar_t* text) const;
	void draw_text_centered(float y, size_t size, const wchar_t* text) const;
};

