#include "color_pallete_btn_panel.h"

#include "../../consts.h"
#include "../../graphic_types/graphic_types.h"
#include "../../input.h"

const Vec2 COLOR_PALLETE_BTN_PANEL_SZ = vec2_new(23, 11);

ColorPalleteBtnPanel color_pallete_btn_panel_new(Vec2 pos) {
	ColorPalleteBtnPanel panel;
	panel.pos = pos;

	panel.copy_btn = btn_new(
		vec2_new(0, 0),
		vec2_new(12, 12),
		BTN_TEXTAREA_COLOR,
		"ICON_COPY"
	);

	panel.paste_btn = btn_new(
		vec2_new(12, 0),
		vec2_new(12, 12),
		BTN_TEXTAREA_COLOR,
		"ICON_FILL_ALL"
	);

	return panel;
}

void color_pallete_btn_panel_update(ColorPalleteBtnPanel &panel,
GraphicStuff &gs, const Input &input, Vec2 parent_pos, bool show) {
	Vec2 pos = vec2_add(parent_pos, panel.pos);
	btn_update(panel.copy_btn, gs, input, pos, show);
	btn_update(panel.paste_btn, gs, input, pos, show);

	if (!show) {
		return;
	}
}

void color_pallete_btn_panel_draw(const ColorPalleteBtnPanel &panel,
GraphicStuff &gs, Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, panel.pos);
	btn_draw(panel.copy_btn, gs, pos, false);
	btn_draw(panel.paste_btn, gs, pos, false);
}
