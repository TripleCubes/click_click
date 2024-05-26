#include "texture.h"

#include <iostream>
#include <string>
#include <stb_image.h>
#include "../types/vec2i.h"
#include "graphic_types.h"

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#else
#include <GLES2/gl2.h>
#endif

bool texture_new(GraphicStuff &gs, const std::string &path) {
	Texture new_texture;
	gs.texture_list.push_back(new_texture);

	int texture_list_sz = (int)gs.texture_list.size();
	Texture &texture = gs.texture_list[texture_list_sz - 1];

	unsigned char *img_data = stbi_load(path.c_str(), &texture.w, &texture.h,
		&texture.num_of_color_channels, 0);
	if (!img_data) {
		std::cout << "cant load img " << path << std::endl;
		return false;
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

	return true;
}

unsigned int texture_get_id(const GraphicStuff &gs, int index) {
	return gs.texture_list[index].id;
}

Vec2i texture_get_sz(const GraphicStuff &gs, int index) {
	const Texture &texture = gs.texture_list[index];
	return vec2i_new(texture.w, texture.h);
}
