#include "basic_draw_op.h"

#include "../types/color.h"

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#else
#include <GLES2/gl2.h>
#endif

void clear(Color color) {
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
}
