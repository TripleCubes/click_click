#ifndef FILE__SAVE_PNG_H
#define FILE__SAVE_PNG_H

#include <string>

struct Tab;
struct OpenProjectData;

bool save_png_from_tab(const std::string &path, const Tab &tab,
	int png_px_scale);

#ifdef __EMSCRIPTEN__
bool web_save_png_from_open_project_data_no_syncfs(const std::string &path,
	const OpenProjectData &open_project_data);
#endif

#endif
