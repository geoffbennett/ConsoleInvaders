#include "keyboard.h"

#include <Windows.h>

void keyboard::sample()
{
	for(auto key = 0; key <= 256; key++)
	{
		new_key_state_[key] = GetAsyncKeyState(key);

		keys_[key].pressed = false;
		keys_[key].released = false;

		if (new_key_state_[key] != old_key_state_[key])
		{
			if (new_key_state_[key] & 0x8000)
			{
				keys_[key].pressed = !keys_[key].held;
				keys_[key].held = true;
			}
			else
			{
				keys_[key].released = true;
				keys_[key].held = false;
			}
		}

		old_key_state_[key] = new_key_state_[key];
	}
}

key_state keyboard::get_key(const int key_id)
{
	return keys_[key_id];
}