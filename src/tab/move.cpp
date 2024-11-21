#include "move.h"

#include <cmath>
#include "../graphic_types/graphic_types.h"
#include "../graphic_types/texture.h"
#include "../input.h"
#include "../pos_convert.h"
#include "tab.h"
#include "history.h"

namespace {

Vec2 prev_tex_draw_mouse_pos = vec2_new(0, 0);

void draw_move_border(Move &move, Tab &tab, unsigned char pallete_index) {
	auto draw = [&tab, pallete_index](int x, int y) {
		if (x < 0 || y < 0 || x >= tab.sz.x || y >= tab.sz.y) {
			return;
		}
		int i = y * tab.sz.x + x;
		tab.tool_preview_data[i] = pallete_index;
	};

	int move_pos_x = std::floor(move.pos.x);
	int move_pos_y = std::floor(move.pos.y);

	for (int x = move_pos_x - 1; x <= move_pos_x + move.sz.x; x++) {
		draw(x, move_pos_y - 1);
		draw(x, move_pos_y + move.sz.y);
	}

	for (int y = move_pos_y - 1; y <= move_pos_y + move.sz.y; y++) {
		draw(move_pos_x - 1, y);
		draw(move_pos_x + move.sz.x, y);
	}
}

}

void move_tool_prepare(Move &move) {
	move.data.clear();
	move.pos = vec2_new(0, 0);
	move.sz = vec2i_new(0, 0);
}

void move_tool_start(Move &move, Tab &tab, const GraphicStuff &gs,
const Input &input, Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, tab.pos);

	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);
	prev_tex_draw_mouse_pos = tex_draw_mouse_pos;

	move.moving = true;
}

void move_tool_commit(Move &move, Tab &tab, GraphicStuff &gs) {
	texture_data_red(gs, tab.move_preview_texture_index,
		tab.sz, tab.move_preview_data);
	texture_data_red(gs, tab.tool_preview_texture_index,
		tab.sz, tab.tool_preview_data);

	int layer_index = tab.layer_order_list[tab.layer_order_list_index];
	Layer &layer = tab.layer_list[layer_index];
	for (int y = 0; y < move.sz.y; y++) {
	for (int x = 0; x < move.sz.x; x++) {
		int data_i = y * move.sz.x + x;
		if (move.data[data_i] == 0) {
			continue;
		}
		int layer_data_x = move.pos.x + x;
		int layer_data_y = move.pos.y + y;
		if (layer_data_x < 0 || layer_data_y < 0
		|| layer_data_x >= tab.sz.x || layer_data_y >= tab.sz.y) {
			continue;
		}
		int layer_data_i = layer_data_y * tab.sz.x + layer_data_x;

		layer.data[layer_data_i] = move.data[data_i];
	}
	}

	texture_data_red(gs, layer.texture_index,
		layer.sz, layer.data);

	history_commit_prepare(tab.history, tab.tab_commands);
	history_commit_layer(
		tab.history, tab.history.layer_list[layer.history_layer_index],
		layer
	);

	move.moving = false;
}

void move_tool_discard(Move &move, Tab &tab, GraphicStuff &gs) {
	texture_data_red(gs, tab.move_preview_texture_index,
		tab.sz, tab.move_preview_data);
	texture_data_red(gs, tab.tool_preview_texture_index,
		tab.sz, tab.tool_preview_data);
	move.moving = false;
}

void move_tool_preview_update(Move &move, Tab &tab,
GraphicStuff &gs, const Input &input, Vec2 parent_pos) {
	if (!move.moving) {
		return;
	}

	for (int y = 0; y < move.sz.y; y++) {
	for (int x = 0; x < move.sz.x; x++) {
		int data_i = y * move.sz.x + x;
		int layer_data_x = move.pos.x + x;
		int layer_data_y = move.pos.y + y;
		if (layer_data_x < 0 || layer_data_y < 0
		|| layer_data_x >= tab.sz.x || layer_data_y >= tab.sz.y) {
			continue;
		}
		int layer_data_i = layer_data_y * tab.sz.x + layer_data_x;

		tab.move_preview_data[layer_data_i] = move.data[data_i];
	}
	}
	texture_data_red(gs, tab.move_preview_texture_index,
		tab.sz, tab.move_preview_data);
	for (int y = 0; y < move.sz.y; y++) {
	for (int x = 0; x < move.sz.x; x++) {
		int layer_data_x = move.pos.x + x;
		int layer_data_y = move.pos.y + y;
		if (layer_data_x < 0 || layer_data_y < 0
		|| layer_data_x >= tab.sz.x || layer_data_y >= tab.sz.y) {
			continue;
		}
		int layer_data_i = layer_data_y * tab.sz.x + layer_data_x;

		tab.move_preview_data[layer_data_i] = 0;
	}
	}

	draw_move_border(move, tab, 1);
	texture_data_red(gs, tab.tool_preview_texture_index,
		tab.sz, tab.tool_preview_data);
	draw_move_border(move, tab, 0);
}

void move_tool_update(Move &move, Tab &tab, const GraphicStuff &gs,
const Input &input, Vec2 parent_pos) {
	if (!move.moving) {
		return;
	}

	Vec2 pos = vec2_add(parent_pos, tab.pos);

	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);

	if (input.left_down) {
		move.pos = vec2_add(
			move.pos,
			vec2_sub(tex_draw_mouse_pos, prev_tex_draw_mouse_pos)
		);
	}

	prev_tex_draw_mouse_pos = tex_draw_mouse_pos;
}
