#include "game_object.h"

game_object::game_object(console_screen* screen, game_timer* timer, keyboard* input)
{
	p_screen_ = screen;
	p_timer_ = timer;
	p_input_ = input;
}
