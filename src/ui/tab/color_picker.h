#ifndef UI__TAB__COLOR_PICKER_H
#define UI__TAB__COLOR_PICKER_H

#include "pos_select.h"
#include "hue_slider.h"
#include "../../types/color.h"

struct Vec2;
struct GraphicStuff;

struct ColorPicker {
	bool show = false;
	bool color_changed = false;
	Vec2 pos;
	PosSelect pos_select;
	HueSlider hue_slider;
};

ColorPicker color_picker_new(Vec2 pos);

void color_picker_update(ColorPicker &color_picker,
	const GraphicStuff &gs, const Input &input, Vec2 parent_pos, bool show);

void color_picker_draw(const ColorPicker &color_picker,
	GraphicStuff &gs, Vec2 parent_pos);

Vec2 color_picker_get_sz(const ColorPicker &color_picker);

Color color_picker_get_hsv(const ColorPicker &color_picker);
Color color_picker_get_rgb(const ColorPicker &color_picker);
void color_picker_set_rgb(ColorPicker &color_picker, Color rgb);

#endif
