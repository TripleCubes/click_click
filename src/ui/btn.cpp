#include "btn.h"

#include "../types/vec2.h"
#include "../types/vec2i.h"
#include "../types/color.h"
#include "../input.h"
#include "../graphic_types/graphic_types.h"
#include "../graphic_types/framebuffer.h"
#include "../graphic/graphic.h"
#include "../graphic/draw_rect.h"

#include "../basic_math.h"
#include "../pos_convert.h"

namespace {

int get_blank_index(const std::vector<Btn> &list) {
	for (int i = 0; i < (int)list.size(); i++) {
		if (!list[i].running) {
			return i;
		}
	}

	return -1;
}

}

int btn_new(std::vector<Btn> &btn_list, Vec2 pos, Vec2 sz, Color color,
const std::string &text, bool show) {
	int index = get_blank_index(btn_list);
	if (index == -1) {
		Btn new_btn;
		btn_list.push_back(new_btn);

		index = (int)btn_list.size() - 1;
	}
	Btn &btn = btn_list[index];

	btn.pos = pos;
	btn.sz = sz;
	btn.color = color;
	btn.text = text;
	btn.show = show;

	return index;
}

void btn_update(Btn &btn, const GraphicStuff &gs, const Input &input) {
	btn.hovered = false;
	btn.clicked = false;
	btn.holding = false;
	btn.released = false;

	Vec2i main_fb_sz = fb_get_sz(gs, FRAMEBUFFER_MAIN);
	Vec2 mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);

	if (!btn.show) {
		return;
	}

	if (!in_rect(mouse_pos, vec2_new(0, 0), to_vec2(main_fb_sz))) {
		return;
	}

	if (!in_rect(mouse_pos, btn.pos, btn.sz)) {
		return;
	}

	btn.hovered = true;

	if (input.left_click) {
		btn.clicked = true;
	}
	if (input.left_down) {
		btn.holding = true;
	}
	if (input.left_release) {
		btn.released = true;
	}
}

void btn_draw(const Btn &btn, const GraphicStuff &gs) {
	if (!btn.show) {
		return;
	}

	Color color = btn.color;
	if (btn.holding) {
		color = color_sub(color, color_new(0.1, 0.1, 0.1, 0));
	}
	else if (btn.hovered) {
		color = color_add(color, color_new(0.1, 0.1, 0.1, 0));
	}

	Vec2i main_fb_sz = fb_get_sz(gs, FRAMEBUFFER_MAIN);
	draw_rect_sz(
		gs,
		main_fb_sz,
		btn.pos,
		btn.sz,
		color
	);
}

void btn_list_update(std::vector<Btn> &btn_list,
const GraphicStuff &gs, const Input &input) {
	for (int i = 0; i < (int)btn_list.size(); i++) {
		if (!btn_list[i].running) {
			continue;
		}
		btn_update(btn_list[i], gs, input);
	}
}

void btn_list_draw(const std::vector<Btn> &btn_list,
const GraphicStuff &gs) {
	for (int i = 0; i < (int)btn_list.size(); i++) {
		if (!btn_list[i].running) {
			continue;
		}
		btn_draw(btn_list[i], gs);
	}
}

bool btn_hovered(const std::vector<Btn> &btn_list, int index) {
	return btn_list[index].hovered;
}

bool btn_clicked(const std::vector<Btn> &btn_list, int index) {
	return btn_list[index].clicked;
}

bool btn_holding(const std::vector<Btn> &btn_list, int index) {
	return btn_list[index].holding;
}

bool btn_released(const std::vector<Btn> &btn_list, int index) {
	return btn_list[index].released;
}
