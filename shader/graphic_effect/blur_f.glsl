#version 330 core
in vec2 f_uv;
out vec4 out_color;

uniform bool u_horizontal;
uniform vec4 u_color;
uniform vec2 u_main_fb_sz;
uniform sampler2D u_texture;

void main() {
	vec4 result = vec4(0.0, 0.0, 0.0, 1.0);

	vec2 px_sz = vec2(1.0/u_main_fb_sz.x, 1.0/u_main_fb_sz.y);

	if (u_horizontal) {
		result += texture(u_texture, vec2(f_uv.x - px_sz.x * 5.0, f_uv.y));
		result += texture(u_texture, vec2(f_uv.x - px_sz.x * 4.0, f_uv.y));
		result += texture(u_texture, vec2(f_uv.x - px_sz.x * 3.0, f_uv.y));
		result += texture(u_texture, vec2(f_uv.x - px_sz.x * 2.0, f_uv.y));
		result += texture(u_texture, vec2(f_uv.x - px_sz.x * 1.0, f_uv.y));
		result += texture(u_texture, vec2(f_uv.x                , f_uv.y));
		result += texture(u_texture, vec2(f_uv.x + px_sz.x * 1.0, f_uv.y));
		result += texture(u_texture, vec2(f_uv.x + px_sz.x * 2.0, f_uv.y));
		result += texture(u_texture, vec2(f_uv.x + px_sz.x * 3.0, f_uv.y));
		result += texture(u_texture, vec2(f_uv.x + px_sz.x * 4.0, f_uv.y));
		result += texture(u_texture, vec2(f_uv.x + px_sz.x * 5.0, f_uv.y));
		result += u_color * 3;
	}
	else {
		result += texture(u_texture, vec2(f_uv.x, f_uv.y - px_sz.y * 5.0));
		result += texture(u_texture, vec2(f_uv.x, f_uv.y - px_sz.y * 4.0));
		result += texture(u_texture, vec2(f_uv.x, f_uv.y - px_sz.y * 3.0));
		result += texture(u_texture, vec2(f_uv.x, f_uv.y - px_sz.y * 2.0));
		result += texture(u_texture, vec2(f_uv.x, f_uv.y - px_sz.y * 1.0));
		result += texture(u_texture, vec2(f_uv.x, f_uv.y                ));
		result += texture(u_texture, vec2(f_uv.x, f_uv.y + px_sz.y * 1.0));
		result += texture(u_texture, vec2(f_uv.x, f_uv.y + px_sz.y * 2.0));
		result += texture(u_texture, vec2(f_uv.x, f_uv.y + px_sz.y * 3.0));
		result += texture(u_texture, vec2(f_uv.x, f_uv.y + px_sz.y * 4.0));
		result += texture(u_texture, vec2(f_uv.x, f_uv.y + px_sz.y * 5.0));
		result += u_color * 3;
	}
	result /= 14.0;
	result.a = 1.0;

	out_color = result;
}
