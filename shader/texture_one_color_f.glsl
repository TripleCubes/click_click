#version 330 core

in vec2 f_uv;
out vec4 out_color;

uniform sampler2D u_texture;
uniform vec4 u_color;
uniform bool u_flip_color;

void main() {
	vec4 color = texture(u_texture, f_uv);
	if (!u_flip_color) {
		if (color.a == 1) {
			out_color = u_color;
		}
		else {
			out_color = vec4(0, 0, 0, 0);
		}
	}
	else {
		if (color.a == 1) {
			out_color = vec4(0, 0, 0, 0);
		}
		else {
			out_color = u_color;
		}
	}
}
