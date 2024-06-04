#version 330 core

in vec2 f_uv;
out vec4 out_color;

uniform sampler2D u_draw_texture;
uniform sampler2D u_pallete_texture;

vec4 get_color(int index) {
	vec2 pos = vec2(index % 16, index / 16);
	pos.x = pos.x / 16.0f + 1/16.0f/2.0f;
	pos.y = pos.y / 16.0f + 1/16.0f/2.0f;
	return texture(u_pallete_texture, pos);
}

void main() {
	int index = int(texture(u_draw_texture, f_uv).r * 255);
	out_color = get_color(index);
}
