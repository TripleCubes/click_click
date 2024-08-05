#include "textarea.h"

#include "../input.h"
#include "../graphic_types/graphic_types.h"
#include "../graphic_types/framebuffer.h"
#include "../graphic/graphic.h"
#include "../graphic/draw_text.h"
#include "../pos_convert.h"
#include "../basic_math.h"
#include "../types/vec2.h"
#include "../types/color.h"

namespace {

bool shift_down(const Input &input) {
	return input.key_list[KEY_LEFT_SHIFT].down
		|| input.key_list[KEY_RIGHT_SHIFT].down;
}

void input_handling(TextArea &textarea, const Input &input) {
	for (int key = KEY_A; key <= KEY_Z; key++) {
		if (input.key_list[key].press) {
			char c = 'a' + (char)key - (char)KEY_A;
			if (shift_down(input)) {
				c -= 32;
			}
			textarea.text += c;
		}
	}

	for (int key = KEY_0; key <= KEY_9; key++) {
		if (input.key_list[key].press) {
			char c = '0' + (char)key - (char)KEY_0;
			textarea.text += c;
		}
	}

	if (input.key_list[KEY_SPACE].press) {
		textarea.text += ' ';
	}
}

}

TextArea textarea_new(Vec2 pos, Vec2 sz, Color color,
const std::string &defl_text) {
	TextArea textarea;
	textarea.pos = pos;
	textarea.sz = sz;
	textarea.color = color;
	textarea.defl_text = defl_text;

	return textarea;
}

void textarea_update(TextArea &textarea, const GraphicStuff &gs,
const Input &input, Vec2 parent_pos, bool active, bool show) {
	textarea.clicked = false;
	textarea.hovered = false;

	if (!show) {
		return;
	}

	if (active) {
		input_handling(textarea, input);
	}

	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);
	Vec2 mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 pos = vec2_add(parent_pos, textarea.pos);

	if (!in_rect(mouse_pos, vec2_new(0, 0), to_vec2(main_fb_sz))) {
		return;
	}

	if (!in_rect(mouse_pos, pos, textarea.sz)) {
		return;
	}

	textarea.hovered = true;

	if (input.left_click) {
		textarea.clicked = true;
	}
}

void textarea_draw(const TextArea &textarea, GraphicStuff &gs, Vec2 parent_pos,
bool active) {
	Vec2 pos = vec2_add(parent_pos, textarea.pos);
	Color color = textarea.color;
	
	bool flip_color = false;

	if (active) {
		flip_color = true;
	}

	draw_text(
		gs,
		textarea.text,
		vec2_add(pos, vec2_new(4, 3)),
		textarea.sz.x - 8,
		1,
		color,
		vec2_new(4, 3),
		flip_color
	);
}
