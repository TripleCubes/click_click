#include "texture.h"

#include <iostream>
#include <string>
#include <vector>
#include <stb_image.h>
#include "../types/vec2i.h"
#include "graphic_types.h"

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#else
#include <GLES2/gl2.h>
#endif

namespace {

int get_blank_index(const std::vector<Texture> &list) {
	for (int i = 0; i < (int)list.size(); i++) {
		if (!list[i].running) {
			return i;
		}
	}

	return -1;
}

int texture_blank_new_format(GraphicStuff &gs, int w, int h, bool red) {
	int index = get_blank_index(gs.texture_list);
	if (index == -1) {
		Texture new_texture;
		gs.texture_list.push_back(new_texture);

		index = (int)gs.texture_list.size() - 1;
	}
	Texture &texture = gs.texture_list[index];

	glGenTextures(1, &texture.id);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	GLint format = GL_RGBA;
	if (red) {
		format = GL_RED;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, format, texture.w, texture.h, 0,
		format, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);

	return index;
}

}

int texture_new(GraphicStuff &gs, const std::string &path) {
	int index = get_blank_index(gs.texture_list);
	if (index == -1) {
		Texture new_texture;
		gs.texture_list.push_back(new_texture);

		index = (int)gs.texture_list.size() - 1;
	}
	Texture &texture = gs.texture_list[index];

	unsigned char *img_data = stbi_load(path.c_str(), &texture.w, &texture.h,
		&texture.num_of_color_channels, 0);
	if (!img_data) {
		std::cout << "cant load img " << path << std::endl;
		return -1;
	}

	glGenTextures(1, &texture.id);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.w, texture.h, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, img_data);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(img_data);

	return index;
}

int texture_blank_new(GraphicStuff &gs, int w, int h) {
	return texture_blank_new_format(gs, w, h, false);
}

int texture_blank_new_red(GraphicStuff &gs, int w, int h) {
	return texture_blank_new_format(gs, w, h, true);
}

unsigned int texture_get_id(const GraphicStuff &gs, int index) {
	return gs.texture_list[index].id;
}

Vec2i texture_get_sz(const GraphicStuff &gs, int index) {
	const Texture &texture = gs.texture_list[index];
	return vec2i_new(texture.w, texture.h);
}

void texture_data(GraphicStuff &gs, int index, Vec2i sz,
const std::vector<unsigned char> &data) {
	Texture &texture = gs.texture_list[index];

	texture.w = sz.x;
	texture.h = sz.y;
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sz.x, sz.y, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, data.data());
}

void texture_data_red(GraphicStuff &gs, int index, Vec2i sz,
const std::vector<unsigned char> &data) {
	Texture &texture = gs.texture_list[index];

	texture.w = sz.x;
	texture.h = sz.y;
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, sz.x, sz.y, 0,
		GL_RED, GL_UNSIGNED_BYTE, data.data());
}

void texture_release(GraphicStuff &gs, int index) {
	Texture &texture = gs.texture_list[index];

	glDeleteTextures(1, &texture.id);
	texture.id = 0;
	texture.running = false;
}
