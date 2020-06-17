#include <iostream>

#include "console_screen.h"

#include "Windows.h"

void console_screen::construct_screen()
{
	p_screen_ = new wchar_t[n_width_ * n_height_];
	for (auto i = 0; i < n_width_ * n_height_; i++) p_screen_[i] = L' ';
	h_screen_ = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);
	SetConsoleActiveScreenBuffer(h_screen_);
	SetConsoleTitle(s_title_);
}

void console_screen::construct_buffer()
{
	p_buffer_ = new wchar_t[n_width_ * n_height_];
}

void console_screen::clip(float& x, float& y) const
{
	if (x < 0.0f) x = 0.0f;
	if (static_cast<int>(x) > n_width_ - 1) x = static_cast<float>(n_width_) - 1;

	if (y < 0.0f) y = 0.0f;
	if (static_cast<int>(y) > n_height_) y = static_cast<float>(n_height_) - 1;
}

console_screen::console_screen(const int width, const int height, const wchar_t* title)
{
	n_width_ = width;
	n_height_ = height;
	s_title_ = title;

	construct_screen();
	construct_buffer();
}

console_screen::~console_screen()
{
	delete[] p_buffer_;
	delete[] p_screen_;
	
	CloseHandle(h_screen_);
}

int console_screen::get_width() const
{
	return n_width_;
}

int console_screen::get_height() const
{
	return n_height_;
}

void console_screen::clear() const
{
	for (auto i = 0; i < n_width_ * n_height_; i++) p_buffer_[i] = L' ';
}

void console_screen::flip() const
{
	for (auto i = 0; i < n_width_ * n_height_; i++) p_screen_[i] = p_buffer_[i];
}

void console_screen::present() const
{
	DWORD dw_bytes_written = 0;
	WriteConsoleOutputCharacter(h_screen_, p_screen_, n_width_ * n_height_, { 0, 0 }, &dw_bytes_written);
}

void console_screen::plot_char(float x, float y, const wchar_t chr) const
{
	clip(x, y);
	p_buffer_[static_cast<int>(y) * n_width_ + static_cast<int>(x)] = chr;
}

void console_screen::draw_text(float x, float y, const size_t size, const wchar_t* text) const
{
	clip(x, y);
	swprintf_s(&p_buffer_[static_cast<int>(y) * n_width_ + static_cast<int>(x)], size, text);
}

void console_screen::draw_text_centered(float y, const size_t size, const wchar_t* text) const
{
	auto left = static_cast<float>(n_width_ - size) / 2.0f;
	clip(left, y);
	swprintf_s(&p_buffer_[static_cast<int>(y) * n_width_ + static_cast<int>(left)], size, text);
}
