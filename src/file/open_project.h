#ifndef FILE__OPEN_PROJECT_H
#define FILE__OPEN_PROJECT_H

#include <string>
#include <vector>
#include <array>

#include "../types/vec2i.h"
#include "../types/vec2.h"
#include "../types/color.h"

struct OpenProjectLayerData {
	std::string layer_name;
	std::vector<unsigned char> data;
};

struct OpenProjectData {
	int ver = 0;
	Vec2i sz;
	Vec2 pos;
	int px_scale;
	std::array<unsigned char, 64 * 4> pallete_data;
	std::vector<OpenProjectLayerData> layer_list;
};

bool file_to_project_data(OpenProjectData &data, const std::string &path);

#endif
