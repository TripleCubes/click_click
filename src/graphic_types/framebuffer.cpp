#include "framebuffer.h"

#include <iostream>
#include <vector>
#include "graphic_types.h"
#include "../types/vec2i.h"

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#else
#include <GLES2/gl2.h>
#endif

namespace {

int get_blank_index(const std::vector<Framebuffer> &list) {
	for (int i = 0; i < (int)list.size(); i++) {
		if (!list[i].running) {
			return i;
		}
	}

	return -1;
}

}

int framebuffer_new(GraphicStuff &gs, int w, int h) {
	int index = get_blank_index(gs.framebuffer_list);
	if (index == -1) {
		Framebuffer new_framebuffer;
		gs.framebuffer_list.push_back(new_framebuffer);

		index = (int)gs.framebuffer_list.size() - 1;
	}
	Framebuffer &framebuffer = gs.framebuffer_list[index];

	framebuffer.w = w;
	framebuffer.h = h;
	
	glGenFramebuffers(1, &framebuffer.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);

	glGenTextures(1, &framebuffer.texture_id);
	glBindTexture(GL_TEXTURE_2D, framebuffer.texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB,
		GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, framebuffer.texture_id, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "cant init framebuffer" << std::endl;
		return -1;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return index;
}

void bind_framebuffer(const GraphicStuff &gs, int index) {
	const Framebuffer &framebuffer = gs.framebuffer_list[index];

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
	glViewport(0, 0, framebuffer.w, framebuffer.h);
}

unsigned int fb_get_texture_id(const GraphicStuff &gs, int index) {
	const Framebuffer &framebuffer = gs.framebuffer_list[index];
	return framebuffer.texture_id;
}

Vec2i fb_get_sz(const GraphicStuff &gs, int index) {
	const Framebuffer &framebuffer = gs.framebuffer_list[index];
	return vec2i_new(framebuffer.w, framebuffer.h);
}

void fb_resize(GraphicStuff &gs, int index, Vec2i sz) {
	Framebuffer &framebuffer = gs.framebuffer_list[index];
	
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
	framebuffer.w = sz.x;
	framebuffer.h = sz.y;
	glBindTexture(GL_TEXTURE_2D, framebuffer.texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sz.x, sz.y, 0,
		GL_RGB, GL_UNSIGNED_BYTE, NULL);
}

void fb_release(GraphicStuff &gs, int index) {
	Framebuffer fb = gs.framebuffer_list[index];

	glDeleteTextures(1, &fb.texture_id);
	glDeleteFramebuffers(1, &fb.fbo);

	fb.texture_id = 0;
	fb.fbo = 0;
	fb.running = false;
}
