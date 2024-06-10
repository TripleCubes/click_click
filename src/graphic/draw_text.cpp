#include "draw_text.h"

#include "../graphic_types/graphic_types.h"
#include "../types/vec2.h"
#include "../types/vec2i.h"
#include "../types/color.h"

#include "draw_texture.h"
#include "../graphic_types/texture.h"
#include "graphic.h"

#include "draw_rect.h"

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

Vec2 get_text_box_sz(const std::string &text, float box_w, int scale) {
	return vec2_new(box_w, 6 * scale);
}

}

void draw_text(const GraphicStuff &gs, Vec2i fb_sz, const std::string &text,
Vec2 pos, float box_w, int scale, Color color, bool flip_color) {
	Vec2 cursor = pos;

	for (int i = 0; i < (int)text.length(); i++) {
		char c = text[i];
		Vec2 char_pos = get_char_pos(c);

		draw_texture_one_color(
			gs,
			texture_get_id(gs, TEXTURE_FONT),

			texture_get_sz(gs, TEXTURE_FONT),
			fb_sz,

			char_pos,
			vec2_new(4, 6),
			
			cursor,
			vec2_mul(vec2_new(4, 6), scale),
			
			true,
			color,
			flip_color
		);

		cursor.x += 4 * scale;
	}
}

void draw_text_with_bkg(const GraphicStuff &gs, Vec2i fb_sz,
const std::string &text, Vec2 pos, float box_w, int scale,
Color color, Vec2 margin, bool flip_color) {
	draw_text(
		gs,
		fb_sz,
		text,
		pos,
		box_w,
		scale,
		color,
		flip_color
	);

	if (!flip_color) {
		return;
	}

	Vec2 box_sz = get_text_box_sz(text, box_w, scale);
	box_sz.y -= 1;

	const int CORNER_PX_SZ = scale;
	draw_rect_sz(
		gs,
		fb_sz,
		vec2_new(pos.x - margin.x + CORNER_PX_SZ, pos.y - margin.y),
		vec2_new(box_sz.x + margin.x * 2 - CORNER_PX_SZ * 2, margin.y),
		color
	);
	draw_rect_sz(
		gs,
		fb_sz,
		vec2_new(pos.x - margin.x + CORNER_PX_SZ, pos.y + box_sz.y),
		vec2_new(box_sz.x + margin.x * 2 - CORNER_PX_SZ * 2, margin.y),
		color
	);
	draw_rect_sz(
		gs,
		fb_sz,
		vec2_new(pos.x - margin.x, pos.y - margin.y + CORNER_PX_SZ),
		vec2_new(margin.x, box_sz.y + margin.y * 2 - CORNER_PX_SZ * 2),
		color
	);
	draw_rect_sz(
		gs,
		fb_sz,
		vec2_new(pos.x + box_sz.x, pos.y - margin.y + CORNER_PX_SZ),
		vec2_new(margin.x, box_sz.y + margin.y * 2 - CORNER_PX_SZ * 2),
		color
	);
}
