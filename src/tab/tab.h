#ifndef TAB__TAB_H
#define TAB__TAB_H

#include <vector>
#include "../types/vec2i.h"
#include "../types/vec2.h"
#include "../ui/tab/color_picker.h"
#include "../ui/tab/color_pallete.h"
#include "../ui/tab/layer_bar.h"
#include "../ui/tab/tool_picker.h"
#include "../ui/tab/btn_panel.h"
#include "../ui/btn.h"
#include "layer.h"
#include "select.h"

struct Color;
struct GraphicStuff;
struct Input;
struct GameTime;
struct Settings;
struct States;

struct Tab {
	Btn btn;
	Btn close_btn;

	bool running = true;
	Vec2 pos;
	Vec2i sz;
	int px_scale = 1;

	Vec2 tex_draw_tag_pos;
	int num_layer_created = 0;
	bool clicked_and_hold_on_ui = false;

	ColorPicker color_picker;
	ColorPallete color_pallete;
	LayerBar layer_bar;
	ToolPicker tool_picker;
	BtnPanel btn_panel;

	std::vector<unsigned char> pallete_data;
	int pallete_texture_index = 0;
	std::vector<unsigned char> tool_preview_pallete_data;
	int tool_preview_pallete_texture_index = 0;

	std::vector<unsigned char> tool_preview_data;
	int tool_preview_texture_index = 0;

	Selection selection;
	std::vector<unsigned char> selection_preview_data;
	int selection_preview_texture_index = 0;

	std::vector<Layer> layer_list;
	std::vector<int> layer_order_list;
	int layer_order_list_index = 0;
	bool layer_name_editing = false;
	bool panning = false;
	bool after_panning_1_frame = false;
	float panning_left_down_at = 0;
	float panning_pan_key_down_at = 0;
};

int tab_new(std::vector<Tab> &tab_list, GraphicStuff &gs,
	Vec2 pos, Vec2i sz, int px_scale);

void tab_update(Tab &tab, GraphicStuff &gs, const States &states,
	const Input &input, const GameTime &game_time, const Settings &settings,
	Vec2 parent_pos, bool show);

void tab_blur_rects_draw(const Tab &tab, GraphicStuff &gs, Vec2 parent_pos);

void tab_bkg_draw(const Tab &tab, GraphicStuff &gs, Vec2 parent_pos);

void tab_canvas_draw(const Tab &tab, GraphicStuff &gs, const Input &input,
	Vec2 parent_pos);

void tab_ui_draw(const Tab &tab, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, Vec2 parent_pos);

void tab_layer_new(Tab &tab, int at, const std::string &layer_name,
	GraphicStuff &gs);

void tab_layer_new_data(Tab &tab, int at, const std::string &layer_name,
	GraphicStuff &gs, const std::vector<unsigned char> &data);

void tab_center_canvas(Tab &tab, const GraphicStuff &gs);

void tab_resize(Tab &tab, GraphicStuff &gs, Vec2i new_pos, Vec2i new_sz);

void tab_close(std::vector<Tab> &tab_list, GraphicStuff &gs, int index);

#endif
