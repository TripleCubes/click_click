#version 330 core
layout (location = 0) in vec2 v_pos;
layout (location = 1) in vec2 v_uv;
layout (location = 2) in vec4 v_color;
layout (location = 3) in float v_flip_color;

out vec2 f_uv;
out vec4 f_color;
out float f_flip_color;

void main() {
	f_uv = vec2(v_uv.x, 1 - v_uv.y);
	f_color = v_color;
	f_flip_color = v_flip_color;
	gl_Position = vec4(v_pos, 0.0, 1.0);
}
