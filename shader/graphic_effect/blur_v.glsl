#version 330 core
layout (location = 0) in vec2 v_pos;

out vec2 f_uv;

uniform bool u_flip;

void main() {
	vec2 pos = v_pos * 2 - vec2(1.0, 1.0);
	if (u_flip) {
		f_uv = vec2(v_pos.x, 1 - v_pos.y);
	}
	else {
		f_uv = v_pos;
	}
	gl_Position = vec4(pos, 0.0, 1.0);
}
