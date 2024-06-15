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
#include "graphic.h"

#include "draw_rect.h"

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

}
