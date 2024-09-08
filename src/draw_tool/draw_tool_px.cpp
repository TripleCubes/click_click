#include "draw_tool_px.h"

#include "../types/vec2i.h"
#include "../tab/select.h"

namespace {

void px(std::vector<unsigned char> &data, const Selection &selection,
bool use_selection,
Vec2i data_sz, Vec2i pos, unsigned char pallete_index, int sz) {
	auto draw = [data_sz, pallete_index, &data, &selection, use_selection]
	(int x, int y) {
		if (x < 0 || y < 0 || x >= data_sz.x || y >= data_sz.y) {
			return;
		}

		int data_index = data_sz.y * y + x;

		if (use_selection && selection.full_preview_list.size() != 0
		&& selection.map[data_index] == 0) {
			return;
		}

		data[data_index] = pallete_index;
	};

	if (sz == 0) {
		draw(pos.x    , pos.y    );
	}

	else if (sz == 1) {
		draw(pos.x    , pos.y    );
		
		draw(pos.x - 1, pos.y    );
		draw(pos.x    , pos.y - 1);
		draw(pos.x - 1, pos.y - 1);
	}

	else if (sz == 2) {
		draw(pos.x, pos.y);

		draw(pos.x - 1, pos.y    );
		draw(pos.x    , pos.y - 1);
		draw(pos.x - 1, pos.y - 1);

		draw(pos.x + 1, pos.y - 1);
		draw(pos.x + 1, pos.y    );
		draw(pos.x + 1, pos.y + 1);
		draw(pos.x - 1, pos.y + 1);
		draw(pos.x    , pos.y + 1);
	}

	else if (sz == 3) {
		draw(pos.x, pos.y);
		
		draw(pos.x - 1, pos.y    );
		draw(pos.x    , pos.y - 1);
		draw(pos.x - 1, pos.y - 1);

		draw(pos.x - 1, pos.y - 2);
		draw(pos.x    , pos.y - 2);
		draw(pos.x - 2, pos.y - 1);
		draw(pos.x - 2, pos.y    );
		draw(pos.x + 1, pos.y - 1);
		draw(pos.x + 1, pos.y    );
		draw(pos.x - 1, pos.y + 1);
		draw(pos.x    , pos.y + 1);
	}

	else if (sz == 4) {
		draw(pos.x, pos.y);
		
		draw(pos.x - 1, pos.y    );
		draw(pos.x    , pos.y - 1);
		draw(pos.x - 1, pos.y - 1);

		draw(pos.x - 1, pos.y - 2);
		draw(pos.x    , pos.y - 2);
		draw(pos.x - 2, pos.y - 1);
		draw(pos.x - 2, pos.y    );
		draw(pos.x + 1, pos.y - 1);
		draw(pos.x + 1, pos.y    );
		draw(pos.x - 1, pos.y + 1);
		draw(pos.x    , pos.y + 1);

		draw(pos.x - 2, pos.y - 2);
		draw(pos.x + 1, pos.y - 2);
		draw(pos.x - 2, pos.y + 1);
		draw(pos.x + 1, pos.y + 1);
	}
}

}

void draw_tool_px(std::vector<unsigned char> &data, const Selection &selection,
Vec2i data_sz, Vec2i pos, unsigned char pallete_index, int sz) {
	px(data, selection, true, data_sz, pos, pallete_index, sz);
}

void draw_tool_px_no_selection(std::vector<unsigned char> &data,
const Selection &selection,
Vec2i data_sz, Vec2i pos, unsigned char pallete_index, int sz) {
	px(data, selection, false, data_sz, pos, pallete_index, sz);
}
