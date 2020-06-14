#pragma once

struct key_state
{
	bool pressed;
	bool held;
	bool released;
};

class keyboard
{
private:
	short old_key_state_[256] = { 0 };
	short new_key_state_[256] = { 0 };
	key_state keys_[256]{};

public:
	void sample();
	key_state get_key(int key_id);
};

