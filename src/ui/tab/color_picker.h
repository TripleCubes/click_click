#ifndef UI__TAB__COLOR_PICKER_H
#define UI__TAB__COLOR_PICKER_H

#include "pos_select.h"
#include "hue_slider.h"
#include "../../types/color.h"
#include "../textarea.h"

struct Vec2;
struct GraphicStuff;
struct GameTime;

struct ColorPicker {
	bool show = false;
	bool color_changed = false;
	Vec2 pos;
	PosSelect pos_select;
	HueSlider hue_slider;
	TextArea hex_ta;
	bool hex_ta_editing = false;
};

ColorPicker color_picker_new(Vec2 pos);

void color_picker_update(ColorPicker &color_picker,
	GraphicStuff &gs, const GameTime &game_time, const Input &input,
	Vec2 parent_pos, bool show);

void color_picker_draw(const ColorPicker &color_picker,
	GraphicStuff &gs, const GameTime &game_time, Vec2 parent_pos);

Vec2 color_picker_get_sz(const ColorPicker &color_picker);

Color color_picker_get_hsv(const ColorPicker &color_picker);
Color color_picker_get_rgb(const ColorPicker &color_picker);
void color_picker_set_rgb(ColorPicker &color_picker, Color rgb);

#endif
