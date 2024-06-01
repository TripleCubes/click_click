#include "graphic_types.h"

#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "framebuffer.h"

void graphic_types_release_all(GraphicStuff &graphic_stuff) {
	for (int i = 0; i < (int)graphic_stuff.mesh_list.size(); i++) {
		if (graphic_stuff.mesh_list[i].running) {
			mesh_release(graphic_stuff, i);
		}
	}

	for (int i = 0; i < (int)graphic_stuff.shader_list.size(); i++) {
		if (graphic_stuff.shader_list[i].running) {
			shader_release(graphic_stuff, i);
		}
	}

	for (int i = 0; i < (int)graphic_stuff.texture_list.size(); i++) {
		if (graphic_stuff.texture_list[i].running) {
			texture_release(graphic_stuff, i);
		}
	}

	for (int i = 0; i < (int)graphic_stuff.framebuffer_list.size(); i++) {
		if (graphic_stuff.framebuffer_list[i].running) {
			fb_release(graphic_stuff, i);
		}
	}
}
