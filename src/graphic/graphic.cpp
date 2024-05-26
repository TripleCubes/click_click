#include "graphic.h"

#include "../graphic_types/graphic_types.h"
#include "../graphic_types/mesh.h"
#include "../graphic_types/shader.h"
#include "../graphic_types/framebuffer.h"
#include "../graphic_types/texture.h"

#include <vector>
#include <string>
#include <iostream>

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


	if (!framebuffer_new(graphic_stuff, 100, 100)) {
		std::cout << "cant init main frambuffer" << std::endl;
		return false;
	}


	if (!texture_new(graphic_stuff, "./texture/font.png")) {
		std::cout << "cant init texture font.png" << std::endl;
		return false;
	}


	return true;
}
