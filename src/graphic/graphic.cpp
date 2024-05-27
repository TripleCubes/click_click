#include "graphic.h"

#include "../graphic_types/graphic_types.h"
#include "../graphic_types/mesh.h"
#include "../graphic_types/shader.h"
#include "../graphic_types/framebuffer.h"
#include "../graphic_types/texture.h"

#include "../types/vec2i.h"

#include <vector>
#include <string>
#include <iostream>

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#else
#include <GLES2/gl2.h>
#endif

namespace {

Vec2i get_main_fb_sz(const GraphicStuff &graphic_stuff) {
	return vec2i_div_div(
		graphic_stuff.current_window_sz,
		graphic_stuff.px_scale
	);
}

}

bool graphic_init(GraphicStuff &graphic_stuff) {
	std::vector<float> verticies_color_rect = {
		0, 1,
		1, 1,
		0, 0,

		1, 1,
		1, 0,
		0, 0,
	};

	if (!mesh_new(graphic_stuff, verticies_color_rect)) {
		std::cout << "cant init color_rect mesh" << std::endl;
		return false;
	}


	#ifndef __EMSCRIPTEN__
	std::string shader_color_rect_path = "./shader/color_rect";
	#else
	std::string shader_color_rect_path = "./shader/color_rect_web";
	#endif
	if (!shader_new(graphic_stuff, shader_color_rect_path)) {
		std::cout << "cant init color_rect shader" << std::endl;
		return false;
	}


	#ifndef __EMSCRIPTEN__
	std::string shader_texture_rect_path = "./shader/texture_rect";
	#else
	std::string shader_texture_rect_path = "./shader/texture_rect_web";
	#endif
	if (!shader_new(graphic_stuff, shader_texture_rect_path)) {
		std::cout << "cant init texture_rect shader" << std::endl;
		return false;
	}

	
	Vec2i main_fb_sz = get_main_fb_sz(graphic_stuff);
	if (!framebuffer_new(graphic_stuff, main_fb_sz.x, main_fb_sz.y)) {
		std::cout << "cant init main frambuffer" << std::endl;
		return false;
	}


	if (!texture_new(graphic_stuff, "./texture/font.png")) {
		std::cout << "cant init texture font.png" << std::endl;
		return false;
	}


	return true;
}

void graphic_resize(GraphicStuff &graphic_stuff, Vec2i sz) {
	if (vec2i_equals(graphic_stuff.current_window_sz, sz)) {
		return;
	}

	graphic_stuff.current_window_sz.x = sz.x;
	graphic_stuff.current_window_sz.y = sz.y;

	Vec2i main_fb_sz = get_main_fb_sz(graphic_stuff);
	fb_resize(graphic_stuff, FRAMEBUFFER_MAIN, main_fb_sz);
}
