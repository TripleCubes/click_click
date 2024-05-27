#ifndef GRAPHIC_TYPES__GRAPHIC_TYPES_H
#define GRAPHIC_TYPES__GRAPHIC_TYPES_H

#include <vector>
#include "../types/vec2i.h"

struct Mesh {
	unsigned int vbo = 0;
	unsigned int vao = 0;
	std::vector<float> verticies;
	int num_of_verticies = 0;
};

struct Shader {
	unsigned int id = 0;
};

struct Texture {
	unsigned int id = 0;
	int w = 0;
	int h = 0;
	int num_of_color_channels = 0;
};

struct Framebuffer {
	unsigned int fbo = 0;
	unsigned int texture_id = 0;
	int w = 0;
	int h = 0;
};

struct GraphicStuff {
	Vec2i current_window_sz;
	int px_scale = 1;
	std::vector<Mesh> mesh_list;
	std::vector<Shader> shader_list;
	std::vector<Texture> texture_list;
	std::vector<Framebuffer> framebuffer_list;
};

void graphic_types_release_all(GraphicStuff &graphic_stuff);

#endif
