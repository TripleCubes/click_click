#ifndef GRAPHIC_TYPES__GRAPHIC_TYPES_H
#define GRAPHIC_TYPES__GRAPHIC_TYPES_H

#include <vector>
#include "../types/vec2i.h"

const int CURSOR_POINTER = 0;
const int CURSOR_TEXT = 1;
const int CURSOR_FILL = 2;
const int CURSOR_MAGIC_WAND = 3;

struct Mesh {
	bool running = true;
	unsigned int vbo = 0;
	unsigned int vao = 0;
	std::vector<float> verticies;
	int num_of_verticies = 0;
};

struct Shader {
	bool running = true;
	unsigned int id = 0;
};

struct Texture {
	bool running = true;
	unsigned int id = 0;
	int w = 0;
	int h = 0;
	int num_of_color_channels = 0;
};

struct Framebuffer {
	bool running = true;
	unsigned int fbo = 0;
	unsigned int texture_id = 0;
	int w = 0;
	int h = 0;
};

struct GraphicStuff {
	Vec2i current_window_sz;
	int px_scale = 1;
	bool just_resized = false;
	bool redraw_requested = false;
	bool draw_secondlayer_ui = false;
	int cursor_icon = CURSOR_POINTER;
	std::vector<Mesh> mesh_list;
	std::vector<Shader> shader_list;
	std::vector<Texture> texture_list;
	std::vector<Framebuffer> framebuffer_list;
};

void graphic_types_release_all(GraphicStuff &graphic_stuff);

#endif
