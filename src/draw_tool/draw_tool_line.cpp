#include "draw_tool_line.h"

#include "draw_tool_px.h"
#include "../types/vec2i.h"
#include "../types/vec2.h"

#include <cmath>

namespace {

Vec2 down_ceil(Vec2 pos, Vec2 dir) {
	float pos_y_ceiled = std::ceil(pos.y);
	float dist_y = pos_y_ceiled - pos.y;
	return vec2_new(pos.x + dist_y / dir.y * dir.x, pos_y_ceiled);
}

Vec2 down(Vec2 pos, Vec2 dir) {
	return vec2_new(pos.x + 1 / dir.y * dir.x, pos.y + 1);
}

Vec2i draw_loop_left_to_right(std::vector<unsigned char> &data, Vec2i data_sz,
Vec2 pos_1, Vec2 pos_2, unsigned char pallete_index) {
	Vec2 diff = vec2_sub(pos_2, pos_1);
	Vec2 start = down_ceil(pos_1, diff);
	Vec2 end = down(start, diff);

	bool first_draw_pos_set = false;
	int first_draw_pos_x = 0;
	int last_draw_pos_x = 0;

	auto draw = [
		&data, data_sz, &first_draw_pos_x, &last_draw_pos_x,&first_draw_pos_set
	](
		Vec2i pos,
		unsigned char pallete_index
	) -> void {
		if (!first_draw_pos_set) {
			first_draw_pos_set = true;
			first_draw_pos_x = pos.x;
		}
		draw_tool_px(data, data_sz, pos, pallete_index);
		last_draw_pos_x = pos.x;
	};

	while (true) {
		float start_x_floored = std::floor(start.x);
		float start_y_floored = std::floor(start.y);
		float end_x_floored = std::floor(end.x);

		if (std::abs(start_x_floored - end_x_floored) > 1) {
			if (start.x - start_x_floored < 0.5) {
				draw(vec2i_new(start_x_floored,start_y_floored),pallete_index);
			}

			for (int i = start_x_floored + 1; i < end_x_floored; i += 1) {
				draw(vec2i_new(i, start_y_floored), pallete_index);
			}

			if (end.x - end_x_floored >= 0.5) {
				draw(vec2i_new(end_x_floored, start_y_floored), pallete_index);
			}
		}

		else if (std::abs(start_x_floored - end_x_floored) == 1) {
			if (start.x - start_x_floored < 0.5
			&& end.x - end_x_floored >= 0.5) {
				draw(vec2i_new(start_x_floored,start_y_floored),pallete_index);
				draw(vec2i_new(end_x_floored, start_y_floored), pallete_index);
			}
			else {
				if (1 - (start.x - start_x_floored) > end.x - end_x_floored) {
					draw(vec2i_new(start_x_floored, start_y_floored),
						pallete_index);
				}
				else {
					draw(vec2i_new(end_x_floored, start_y_floored),
						pallete_index);
				}
			}
		}

		else {
			draw(vec2i_new(start_x_floored, start_y_floored), pallete_index);
		}

		start = end;
		end = down(start, diff);
		if (end.y > pos_2.y) {
			break;
		}
	}

	return vec2i_new(first_draw_pos_x, last_draw_pos_x);
}

Vec2i draw_loop_right_to_left(std::vector<unsigned char> &data, Vec2i data_sz,
Vec2 pos_1, Vec2 pos_2, unsigned char pallete_index) {
	Vec2 diff = vec2_sub(pos_2, pos_1);
	Vec2 start = down_ceil(pos_1, diff);
	Vec2 end = down(start, diff);

	bool first_draw_pos_set = false;
	int first_draw_pos_x = 0;
	int last_draw_pos_x = 0;

	auto draw = [
		&data, data_sz, &first_draw_pos_x, &last_draw_pos_x,&first_draw_pos_set
	](
		Vec2i pos,
		unsigned char pallete_index
	) -> void {
		if (!first_draw_pos_set) {
			first_draw_pos_set = true;
			first_draw_pos_x = pos.x;
		}
		draw_tool_px(data, data_sz, pos, pallete_index);
		last_draw_pos_x = pos.x;
	};

	while (true) {
		float start_x_floored = std::floor(start.x);
		float start_y_floored = std::floor(start.y);
		float end_x_floored = std::floor(end.x);

		if (std::abs(start_x_floored - end_x_floored) > 1) {
			if (start.x - start_x_floored >= 0.5) {
				draw(vec2i_new(start_x_floored,start_y_floored),pallete_index);
			}

			for (int i = start_x_floored - 1; i > end_x_floored; i -= 1) {
				draw(vec2i_new(i, start_y_floored), pallete_index);
			}

			if (end.x - end_x_floored < 0.5) {
				draw(vec2i_new(end_x_floored, start_y_floored), pallete_index);
			}
		}

		else if (std::abs(start_x_floored - end_x_floored) == 1) {
			if (start.x - start_x_floored >= 0.5
			&& end.x - end_x_floored < 0.5) {
				draw(vec2i_new(start_x_floored,start_y_floored),pallete_index);
				draw(vec2i_new(end_x_floored, start_y_floored), pallete_index);
			}
			else {
				if (start.x - start_x_floored > 1 - (end.x - end_x_floored)) {
					draw(vec2i_new(start_x_floored, start_y_floored),
						pallete_index);
				}
				else {
					draw(vec2i_new(end_x_floored, start_y_floored),
						pallete_index);
				}
			}
		}

		else {
			draw(vec2i_new(start_x_floored, start_y_floored), pallete_index);
		}

		start = end;
		end = down(start, diff);
		if (end.y > pos_2.y) {
			break;
		}
	}

	return vec2i_new(first_draw_pos_x, last_draw_pos_x);
}

void line_down(std::vector<unsigned char> &data, Vec2i data_sz,
Vec2 pos_1, Vec2 pos_2, unsigned char pallete_index) {
	Vec2i draw_result;

	if (pos_1.x < pos_2.x) {
		draw_result = draw_loop_left_to_right(
			data, data_sz, pos_1, pos_2, pallete_index
		);
	}
	else {
		draw_result = draw_loop_right_to_left(
			data, data_sz, pos_1, pos_2, pallete_index
		);
	}

	int first_draw_pos_x = draw_result.x;
	int last_draw_pos_x = draw_result.y;

	if (pos_1.x < pos_2.x) {
		for (int i = std::floor(pos_1.x); i < first_draw_pos_x; i++) {
			draw_tool_px(data, data_sz,
				vec2i_new(i, std::floor(pos_1.y)), pallete_index);
		}
		for (int i = last_draw_pos_x + 1; i <= std::floor(pos_2.x); i++) {
			draw_tool_px(data, data_sz,
				vec2i_new(i, std::floor(pos_2.y)), pallete_index);
		}
	}
	else {
		for (int i = first_draw_pos_x + 1; i <= std::floor(pos_1.x); i++) {
			draw_tool_px(data, data_sz,
				vec2i_new(i, std::floor(pos_1.y)), pallete_index);
		}
		for (int i = std::floor(pos_2.x); i < last_draw_pos_x; i++) {
			draw_tool_px(data, data_sz,
				vec2i_new(i, std::floor(pos_2.y)), pallete_index);
		}
	}
}

}

void draw_tool_line(std::vector<unsigned char> &data, Vec2i data_sz,
Vec2 pos_1, Vec2 pos_2, unsigned char pallete_index) {
	Vec2i pos_1_i = to_vec2i(pos_1);
	Vec2i pos_2_i = to_vec2i(pos_2);

	if (pos_1_i.x == pos_2_i.x && pos_1_i.y == pos_2_i.y) {
		draw_tool_px(data, data_sz, pos_1_i, pallete_index);
		return;
	}

	if (pos_1.y > pos_2.y) {
		Vec2 swap = pos_1;
		pos_1 = pos_2;
		pos_2 = swap;

		Vec2i swapi = pos_1_i;
		pos_1_i = pos_2_i;
		pos_2_i = swapi;
	}

	if (pos_1_i.x == pos_2_i.x) {
		for (int i = pos_1_i.y; i <= pos_2_i.y; i++) {
			draw_tool_px(data, data_sz,
				vec2i_new(pos_1_i.x, i), pallete_index);
		}
		return;
	}

	if (pos_1_i.y == pos_2_i.y) {
		if (pos_1_i.x > pos_2_i.x) {
			Vec2i swapi = pos_1_i;
			pos_1_i = pos_2_i;
			pos_2_i = swapi;
		}

		for (int i = pos_1_i.x; i <= pos_2_i.x; i++) {
			draw_tool_px(data, data_sz,
				vec2i_new(i, pos_1_i.y), pallete_index);
		}

		return;
	}

	if (pos_2_i.y - pos_1_i.y == 1) {
		Vec2 separator = down_ceil(pos_1, vec2_sub(pos_2, pos_1));
		Vec2i separatori = to_vec2i(separator);

		int y = pos_1_i.y;
		int x = pos_1_i.x;
		if (pos_1_i.x > pos_2_i.x) {
			y = pos_2_i.y;
			x = pos_2_i.x;
		}

		for (int i = x; i < separatori.x; i++) {
			draw_tool_px(data, data_sz, vec2i_new(i, y), pallete_index);
		}

		y = pos_2_i.y;
		x = pos_2_i.x;
		if (pos_1_i.x > pos_2_i.x) {
			y = pos_1_i.y;
			x = pos_1_i.x;
		}

		for (int i = separatori.x + 1; i <= x; i++) {
			draw_tool_px(data, data_sz, vec2i_new(i, y), pallete_index);
		}

		Vec2i draw_pos;
		draw_pos.x = separatori.x;
		if (separator.x - std::floor(separator.x) > 0.5) {
			if (pos_1_i.x <= pos_2_i.x) {
				draw_pos.y = pos_1_i.y;
			}
			else {
				draw_pos.y = pos_2_i.y;
			}
		}
		else {
			if (pos_1_i.x > pos_2_i.x) {
				draw_pos.y = pos_1_i.y;
			}
			else {
				draw_pos.y = pos_2_i.y;
			}
		}

		draw_tool_px(data, data_sz, draw_pos, pallete_index);

		return;
	}

	line_down(data, data_sz, pos_1, pos_2, pallete_index);
}
