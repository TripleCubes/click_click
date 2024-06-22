#include "draw_text.h"

#include <vector>

#include "../graphic_types/graphic_types.h"
#include "../types/vec2.h"
#include "../types/vec2i.h"
#include "../types/color.h"

#include "draw_texture.h"
#include "../graphic_types/texture.h"
#include "../graphic_types/mesh.h"
#include "../graphic_types/shader.h"
#include "../graphic_types/framebuffer.h"
#include "graphic.h"

#include "draw_rect.h"

#include <iostream>

namespace {

const int CHAR_W = 4;
const int REAL_CHAR_H = 6;
const int CHAR_H = 8;

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

Vec2 get_boxed_text(std::string &result, const std::string &text,
float box_w, int scale) {
	result = text;

	int num_line = 1;

	int last_line_break = 0;
	int current_word_start = 0;

	for (int i = 0; i < (int)result.length(); i++) {
		char c = result[i];
		
		float w = (i - last_line_break + 1) * CHAR_W * scale;
	
		if (c == ' ') {
			current_word_start = i + 1;
			continue;
		}
	
		if (w > box_w && current_word_start - 1 >= 0) {
			result[current_word_start - 1] = '\n';
			last_line_break = current_word_start;
			num_line += 1;
		}
	}

	return vec2_new(box_w, num_line * CHAR_H * scale);
}

void str_split_lines(std::vector<std::string> &result,
const std::string &str) {
	std::string current_line;

	for (int i = 0; i < (int)str.length(); i++) {
		char c = str[i];
		
		if (c == '\n') {
			result.push_back(current_line);
			current_line = "";
			continue;
		}

		current_line += c;
	}

	result.push_back(current_line);
}

void draw_text_one_line(GraphicStuff &gs,
const std::string &text, Vec2 pos, float box_w, int scale,
Color color, bool flip_color, bool fill_bottom) {
	Vec2 cursor = pos;

	for (int i = 0; i < (int)text.length(); i++) {
		char c = text[i];
		Vec2 char_pos = get_char_pos(c);

		draw_texture(
			gs,
			texture_get_sz(gs, TEXTURE_FONT),

			char_pos,
			vec2_new(4, 6),
			
			cursor,
			vec2_mul(vec2_new(4, 6), scale),

			color,
			flip_color
		);

		cursor.x += CHAR_W * scale;
	}

	float filled_w = (int)text.length() * CHAR_W * scale;

	if (!flip_color) {
		return;
	}

	float h = CHAR_H;
	if (!fill_bottom) {
		h = REAL_CHAR_H;
	}

	draw_rect(
		gs,
		vec2_new(pos.x + filled_w, pos.y),
		vec2_new(box_w - filled_w, h * scale),
		color
	);

	if (!fill_bottom) {
		return;
	}

	draw_rect(
		gs,
		vec2_new(pos.x, pos.y + REAL_CHAR_H),
		vec2_new(box_w, CHAR_H - REAL_CHAR_H),
		color
	);
}

Vec2 draw_text_no_bkg(GraphicStuff &gs,
const std::string &text, Vec2 pos, float box_w, int scale, Color color,
bool flip_color) {
	std::string boxed_text;
	Vec2 box_sz = get_boxed_text(boxed_text, text, box_w, scale);

	std::vector<std::string> line_list;
	str_split_lines(line_list, boxed_text);

	Vec2 cursor = pos;

	for (int i = 0; i < (int)line_list.size(); i++) {
		bool fill_bottom = true;
		if (i == (int)line_list.size() - 1) {
			fill_bottom = false;
		}

		draw_text_one_line(
			gs,
			line_list[i],
			cursor,
			box_w,
			scale,
			color,
			flip_color,
			fill_bottom
		);
		cursor.y += CHAR_H * scale;
	}

	return box_sz;
}

}

Vec2 draw_text(GraphicStuff &gs,
const std::string &text, Vec2 pos, float box_w, int scale,
Color color, Vec2 bkg_margin, bool flip_color) {
	Vec2 box_sz = draw_text_no_bkg(
		gs,
		text,
		pos,
		box_w,
		scale,
		color,
		flip_color
	);

	if (!flip_color) {
		return box_sz;
	}

	box_sz.y -= CHAR_H - REAL_CHAR_H + 1;

	const int CORNER_PX_SZ = scale;
	draw_rect(
		gs,
		vec2_new(pos.x - bkg_margin.x + CORNER_PX_SZ, pos.y - bkg_margin.y),
		vec2_new(box_sz.x + bkg_margin.x * 2 - CORNER_PX_SZ * 2, bkg_margin.y),
		color
	);
	draw_rect(
		gs,
		vec2_new(pos.x - bkg_margin.x + CORNER_PX_SZ, pos.y + box_sz.y),
		vec2_new(box_sz.x + bkg_margin.x * 2 - CORNER_PX_SZ * 2, bkg_margin.y),
		color
	);
	draw_rect(
		gs,
		vec2_new(pos.x - bkg_margin.x, pos.y - bkg_margin.y + CORNER_PX_SZ),
		vec2_new(bkg_margin.x, box_sz.y + bkg_margin.y * 2 - CORNER_PX_SZ * 2),
		color
	);
	draw_rect(
		gs,
		vec2_new(pos.x + box_sz.x, pos.y - bkg_margin.y + CORNER_PX_SZ),
		vec2_new(bkg_margin.x, box_sz.y + bkg_margin.y * 2 - CORNER_PX_SZ * 2),
		color
	);

	return box_sz;
}
