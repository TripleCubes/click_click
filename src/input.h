#ifndef INPUT_H
#define INPUT_H

#include "types/vec2.h"

struct Input {
	Vec2 mouse_pos;
	bool left_click = false;
	bool left_down = false;
	bool right_click = false;
	bool right_down = false;
};

#endif
