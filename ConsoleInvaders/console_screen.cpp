#include "console_screen.h"

console_screen::console_screen(int width, int height, const wchar_t* title)
{
	n_width_ = width;
	n_height_ = height;
	SetConsoleTitle(title);
}

console_screen::~console_screen()
{
	cleanup();
}

void console_screen::clip(int& x, int& y) const
{
	if (x < 0) x = 0;
	if (x > n_width_ - 1) x = n_width_ - 1;

	if (y < 0) y = 0;
	if (y > n_height_) y = n_height_ - 1;
}

void console_screen::console_initialise()
{
	h_console_ = GetStdHandle(STD_OUTPUT_HANDLE);
	r_console_ = { 0, 0, static_cast<short>(n_width_) - 1, static_cast<short>(n_height_) - 1 };
}

void console_screen::clear() const
{
	memset(p_screen_buffer_, 0, sizeof(CHAR_INFO) * n_width_ * n_height_);
}

void console_screen::initialise()
{
	p_screen_buffer_ = new CHAR_INFO[n_width_ * n_height_];
	clear();
}

void console_screen::cleanup() const
{
	delete[] p_screen_buffer_;
}

void console_screen::present()
{
	WriteConsoleOutput(h_console_, p_screen_buffer_, { static_cast<short>(n_width_), static_cast<short>(n_height_) }, { 0,0 }, &r_console_);
}

CHAR_INFO* console_screen::peek(const int x, const int y) const
{
	return &p_screen_buffer_[y * n_width_ + x];
}

void console_screen::draw(int x, int y, const short c = 0x2588, const short col = 0x000f) const
{
	clip(x, y);
	p_screen_buffer_[y * n_width_ + x].Char.UnicodeChar = c;
	p_screen_buffer_[y * n_width_ + x].Attributes = col;
}

void console_screen::draw_text(const int x, const int y, std::wstring c, const short col = 0x000F) const
{
	for (size_t i = 0; i < c.size(); i++)
	{
		p_screen_buffer_[y * n_width_ + x + i].Char.UnicodeChar = c[i];
		p_screen_buffer_[y * n_width_ + x + i].Attributes = col;
	}
}

void console_screen::draw_text_centered(const int y, const std::wstring& c, const short col = 0x000F) const
{
	const auto x = (n_width_ - c.size()) / 2;
	draw_text(x, y, c, col);
}
