#include "textarea.h"

#include "../input.h"
#include "../game_time.h"
#include "../basic_math.h"
#include "../pos_convert.h"
#include "../consts.h"
#include "../graphic_types/graphic_types.h"
#include "../graphic_types/framebuffer.h"
#include "../graphic/graphic.h"
#include "../graphic/draw_text.h"
#include "../graphic/draw_rect.h"
#include "../pos_convert.h"
#include "../basic_math.h"
#include "../types/vec2.h"
#include "../types/color.h"

#include <cmath>

namespace {

const Vec2 TEXT_MARGIN = vec2_new(4, 3);

void text_input_handling(TextArea &textarea, const GameTime &game_time,
const Input &input) {
	char add = ' ';
	bool text_input = false;

	for (int key = KEY_A; key < KEY_COUNT; key++) {
		if (input.key_list[key].press) {
			char c = key_get_char_no_special(key, shift_down(input));
			if (c != ' ') {
				add = c;
				text_input = true;
			}
		}
	}

	if (input.key_list[KEY_SPACE].press) {
		add = ' ';
		text_input = true;
	}

	if (text_input) {
		const int TEXT_W_CHAR
			= std::floor(textarea.sz.x / (1 * CHAR_W)) - 2;

		if (textarea.cursor_at < TEXT_W_CHAR) {
			if (textarea.cursor_at == (int)textarea.text.length()) {
				textarea.text += add;
			}

			else {
				textarea.text.insert(textarea.cursor_at, 1, add);
			}

			textarea.cursor_at++;
			textarea.cursor_moved_at = game_time.time_since_start;
		}
	}

	if (input.key_list[KEY_BACKSPACE].press && textarea.cursor_at > 0) {
		if (textarea.cursor_at == (int)textarea.text.length()) {
			textarea.text.pop_back();
		}

		else {
			textarea.text.erase(textarea.cursor_at - 1, 1);
		}

		textarea.cursor_at--;
	}
}

void cursor_move_handling(TextArea &textarea, const GameTime &game_time,
const Input &input) {
	if (input.key_list[KEY_LEFT].press) {
		textarea.cursor_at
			= clampi(textarea.cursor_at - 1, 0, textarea.text.length());
		textarea.cursor_moved_at = game_time.time_since_start;
	}

	else if (input.key_list[KEY_RIGHT].press) {
		textarea.cursor_at
			= clampi(textarea.cursor_at + 1, 0, textarea.text.length());
		textarea.cursor_moved_at = game_time.time_since_start;
	}

	else if (input.key_list[KEY_UP].press) {
		textarea.cursor_at = 0;
		textarea.cursor_moved_at = game_time.time_since_start;
	}

	else if (input.key_list[KEY_DOWN].press) {
		textarea.cursor_at = (int)textarea.text.length();
		textarea.cursor_moved_at = game_time.time_since_start;
	}
}

void input_handling(TextArea &textarea, const GameTime &game_time,
const Input &input) {
	text_input_handling(textarea, game_time, input);
	cursor_move_handling(textarea, game_time, input);
}

}

TextArea textarea_new(Vec2 pos, Vec2 sz, Color color,
const std::string &defl_text) {
	TextArea textarea;
	textarea.pos = pos;
	textarea.sz = sz;
	textarea.color = color;
	textarea.defl_text = defl_text;
	textarea.text = defl_text;

	return textarea;
}

void textarea_update(TextArea &textarea, const GraphicStuff &gs,
const GameTime &game_time, const Input &input, Vec2 parent_pos,
bool active, bool show) {
	textarea.clicked = false;
	textarea.hovered = false;

	if (!show) {
		return;
	}

	if (active) {
		input_handling(textarea, game_time, input);
	}

	Vec2 mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 pos = vec2_add(parent_pos, textarea.pos);

	if (!in_rect(mouse_pos, pos, textarea.sz)) {
		return;
	}

	textarea.hovered = true;

	if (input.left_click) {
		textarea.clicked = true;
		textarea.cursor_moved_at = game_time.time_since_start;

		Vec2 clicked_pos = vec2_sub(mouse_pos, pos);
		int click_x_char
			= std::floor((clicked_pos.x - TEXT_MARGIN.x + 2) / CHAR_W /1);
		click_x_char = clampi(click_x_char, 0, (int)textarea.text.length());
		textarea.cursor_at = click_x_char;
	}
}

void textarea_draw(const TextArea &textarea, GraphicStuff &gs,
const GameTime &game_time, Vec2 parent_pos, bool active, bool selected) {
	Vec2 pos = vec2_add(parent_pos, textarea.pos);
	Color color = textarea.color;
	
	bool flip_color = false;

	if (selected || textarea.hovered) {
		flip_color = true;
	}

	draw_text(
		gs,
		textarea.text,
		vec2_add(pos, TEXT_MARGIN),
		textarea.sz.x - TEXT_MARGIN.x * 2,
		1,
		color,
		TEXT_MARGIN,
		flip_color
	);

	if (!active) {
		return;
	}

	if (mod2(game_time.time_since_start - textarea.cursor_moved_at, 2) > 1) {
		return;
	}

	draw_rect(
		gs,
		vec2_add(
			pos, vec2_add(
				TEXT_MARGIN, vec2_new(
					textarea.cursor_at * CHAR_W - 1, 5
				)
			)
		),
		vec2_new(5, 1),
		color_new(0.83, 1, 0.76, 1)
	);
}
