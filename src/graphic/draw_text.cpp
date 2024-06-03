#include "draw_text.h"

#include "../graphic_types/graphic_types.h"
#include "../types/vec2.h"
#include "../types/vec2i.h"
#include "../types/color.h"

#include "draw_texture.h"
#include "../graphic_types/texture.h"
#include "graphic.h"

#include <iostream>

namespace {

Vec2 get_char_pos(char c) {
	if (c >= 'A' && c <= 'P') {
		return vec2_new((c - 'A') * 4, 0);
	}
	if (c >= 'Q' && c <= 'Z') {
		return vec2_new((c - 'Q') * 4, 6);
	}

	else if (c >= 'a' && c <= 'p') {
		return vec2_new((c - 'a') * 4, 12);
	}
	else if (c >= 'q' && c <= 'z') {
		return vec2_new((c - 'q') * 4, 18);
	}

	else if (c >= '0' && c <= '9') {
		return vec2_new((c - '0') * 4, 24);
	}

	else if (c >= ' ' && c <= '/') {
		return vec2_new((c - ' ') * 4, 30);
	}
	else if (c >= ':' && c <= '@') {
		return vec2_new((c - ':') * 4, 36);
	}

	else if (c >= '[' && c <= '`') {
		return vec2_new((c - '[') * 4 + 28, 36);
	}

	else if (c >= '{' && c <= '~') {
		return vec2_new((c - '{') * 4, 42);
	}

	return vec2_new(60, 6);
}
	
}

void draw_text(const GraphicStuff &gs, const std::string &text,
Vec2i fb_sz, Vec2 pos, Vec2 box_sz, int scale, Color color) {
	Vec2 cursor = pos;

	for (int i = 0; i < (int)text.length(); i++) {
		char c = text[i];
		Vec2 char_pos = get_char_pos(c);

		draw_texture(
			gs,

			texture_get_sz(gs, TEXTURE_FONT),
			fb_sz,

			char_pos,
			vec2_new(4, 6),
			
			cursor,
			vec2_mul(vec2_new(4, 6), scale),
			
			texture_get_id(gs, TEXTURE_FONT),
			true
		);

		cursor.x += 4 * scale;
	}
}
