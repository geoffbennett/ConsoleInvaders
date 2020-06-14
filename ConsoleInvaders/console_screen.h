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
	console_screen(const int width, const int height, const wchar_t* title);
	~console_screen();

	void clear() const;
	void flip() const;
	void present() const;

	void plot_char(float x, float y, const wchar_t chr) const;
	void draw_text(float x, float y, const size_t size, const wchar_t* text) const;
	void draw_text_centered(float y, const size_t size, const wchar_t* text) const;

};

