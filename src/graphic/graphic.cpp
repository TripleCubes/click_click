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

std::string shader_path(const std::string &path) {
	#ifndef __EMSCRIPTEN__
	return path;
	#else
	return path + "_web";
	#endif
}

}

bool graphic_init(GraphicStuff &graphic_stuff) {
	if (mesh_new(graphic_stuff) == -1) {
		std::cout << "cant init rect mesh" << std::endl;
		return false;
	}
	std::vector<float> rect_verticies = {
		0, 1, -1, -1, 0, 0, 0, 0,  0,
		1, 1, -1, -1, 0, 0, 0, 0,  0,
		0, 0, -1, -1, 0, 0, 0, 0,  0,

		1, 1, -1, -1, 0, 0, 0, 0,  0,
		1, 0, -1, -1, 0, 0, 0, 0,  0,
		0, 0, -1, -1, 0, 0, 0, 0,  0,
	};
	mesh_add(graphic_stuff, MESH_RECT, rect_verticies);
	mesh_set(graphic_stuff, MESH_RECT);

	if (mesh_new(graphic_stuff) == -1) {
		std::cout << "cant init basic_draw mesh" << std::endl;
		return false;
	}


	Vec2i main_fb_sz = get_main_fb_sz(graphic_stuff);
	if (framebuffer_new(graphic_stuff, main_fb_sz.x, main_fb_sz.y) == -1) {
		std::cout << "cant init main frambuffer" << std::endl;
		return false;
	}


	if (shader_new(graphic_stuff, shader_path("./shader/basic_draw")) == -1) {
		std::cout << "cant init shader basic_draw" << std::endl;
		return false;
	}

	if (shader_new(graphic_stuff, shader_path("./shader/fb_main")) == -1) {
		std::cout << "cant init shader fb_main" << std::endl;
		return false;
	}

	if (shader_new(graphic_stuff, shader_path("./shader/tab/layer_draw"))==-1){
		std::cout << "cant init shader layer_draw" << std::endl;
		return false;
	}

	if (shader_new(graphic_stuff,
	               shader_path("./shader/color_picker/pos_select")) == -1) {
		std::cout << "cant init shader color_picker_pos_select" << std::endl;
		return false;
	}

	if (shader_new(graphic_stuff,
		           shader_path("./shader/color_picker/hue_slider")) == -1) {
		std::cout << "cant init shader color_picker_hue_slider" << std::endl;
		return false;
	}


	if (texture_new(graphic_stuff, "./texture/font.png") == -1) {
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
	fb_resize(graphic_stuff, FB_MAIN, main_fb_sz);
}
