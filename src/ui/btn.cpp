#include "btn.h"

#include "../types/vec2.h"
#include "../types/vec2i.h"
#include "../types/color.h"
#include "../input.h"
#include "../graphic_types/graphic_types.h"
#include "../graphic_types/framebuffer.h"
#include "../graphic/graphic.h"
#include "../graphic/draw_rect.h"
#include "../graphic/draw_text.h"

#include "../basic_math.h"
#include "../pos_convert.h"
#include "../consts.h"

#include <array>

Btn btn_new(Vec2 pos, Vec2 sz, Color color, const std::string &text) {
	Btn btn;
	btn.pos = pos;
	btn.sz = sz;
	btn.color = color;
	btn.text = text;

	return btn;
}

void btn_update(Btn &btn, const GraphicStuff &gs, const Input &input,
Vec2 parent_pos, bool show) {
	btn.hovered = false;
	btn.clicked = false;

	if (!show) {
		btn.holding = false;
		return;
	}

	Vec2 mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 pos = vec2_add(parent_pos, btn.pos);

	if (input.left_release) {
		btn.holding = false;
	}

	if (!in_rect(mouse_pos, pos, btn.sz)) {
		return;
	}

	btn.hovered = true;

	if (input.left_click) {
		btn.holding = true;
		btn.clicked = true;
	}
}

void btn_draw(const Btn &btn, GraphicStuff &gs, Vec2 parent_pos,
bool selected, const std::string &replace_text) {
	Vec2 pos = vec2_add(parent_pos, btn.pos);
	Color color = btn.color;
	
	bool flip_color = false;

	std::string text;
	if (replace_text.length() != 0) {
		text = replace_text;
	}
	else {
		text = btn.text;
	}

	if (btn.holding || btn.hovered || selected) {
		flip_color = true;
	}

	auto _draw_icon = [&gs, color, flip_color, pos](int icon) {
		draw_icon(
			gs,
			icon,
			vec2_add(pos, vec2_new(3, 3)),
			1,
			color,
			vec2_new(3, 3),
			flip_color
		);
	};

	std::array<std::string, ICON_COUNT> icon_str_list = {
		"ICON_PLUS",
		"ICON_X",
		"ICON_MINUS",
		"ICON_CURVE",
		"ICON_FILL",
		"ICON_PEN",
		"ICON_SELECT",
		"ICON_UP",
		"ICON_DOWN",
		"ICON_SZ_1",
		
		"ICON_SZ_2",
		"ICON_SZ_3",
		"ICON_SZ_3_5",
		"ICON_SZ_4",
		"ICON_FILL_ALL",
		"ICON_FILL_DITHERED",
		"ICON_FREE_SELECT",
		"ICON_MAGIC_WAND",
		"ICON_SELECT_ADD",
		"ICON_SELECT_SUBTRACT",
		
		"ICON_ARROW_UP",
		"ICON_FILE",
		"ICON_FOLDER",
		"ICON_IMAGE",
		"ICON_SZ_4_5",
		"ICON_PIN",
		"ICON_CIRCLE",

		"ICON_HAMBURGER",
		"ICON_ERASER",
		"ICON_DOWNLOAD",

		"ICON_UNLOCKED",
		"ICON_MOVE",
		"ICON_LOCKED",

		"ICON_COPY",
	};

	for (int i = 0; i < ICON_COUNT; i++) {
		if (text == icon_str_list[i]) {
			_draw_icon(i);
			return;
		}
	}

	draw_text(
		gs,
		text,
		vec2_add(pos, vec2_new(4, 3)),
		btn.sz.x - 8,
		1,
		color,
		vec2_new(4, 3),
		flip_color
	);
}
