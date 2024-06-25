#ifndef UI__HUE_SLIDER_H
#define UI__HUE_SLIDER_H

#include "../../types/vec2.h"

struct GraphicStuff;
struct Input;

struct HueSlider {
	bool sliding = false;
	Vec2 pos;
	Vec2 sz;
	float slide_percentage = 0;
};

HueSlider hue_slider_new(Vec2 pos, Vec2 sz);
void hue_slider_update(HueSlider &hue_slider,
	const GraphicStuff &gs, const Input &input, Vec2 parent_pos, bool show);
void hue_slider_draw(const HueSlider &hue_slider,
	GraphicStuff &gs, Vec2 parent_pos);

#endif
