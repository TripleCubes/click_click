#ifndef UI__DIALOG_BOX__DIALOG_BOX_H
#define UI__DIALOG_BOX__DIALOG_BOX_H

#include <string>
#include "../../types/vec2.h"
#include "../btn.h"

const int DIALOG_BOX_WEB_DELETE_FILE = 0;

struct GraphicStuff;
struct Input;
struct GameTime;

struct DialogBox {
	Vec2 sz;

	std::string title_text;
	std::string text;

	Btn ok_btn;
	Btn cancel_btn;

	int dialog_type = DIALOG_BOX_WEB_DELETE_FILE;

	#ifdef __EMSCRIPTEN__
	std::string web_delete_file_name;
	#endif
};

void dialog_box_init(DialogBox &dialog_box);

void dialog_box_update(DialogBox &dialog_box, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, Vec2 parent_pos, bool show);

void dialog_box_draw(const DialogBox &dialog_box, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, Vec2 parent_pos);

void dialog_box_bkg_draw(const DialogBox &dialog_box, GraphicStuff &gs,
	Vec2 parent_pos);

void dialog_box_set(DialogBox &dialog_box, int dialog_type);

#endif
