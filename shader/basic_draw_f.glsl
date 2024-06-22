#version 330 core
in vec2 f_uv;
in vec4 f_color;
in float f_flip_color;
out vec4 out_color;

uniform sampler2D u_texture;

void main() {
	bool is_texture_draw = f_uv.x != -1.0;
	bool is_color = f_color.a != 0.0;

	vec4 texture_color = texture(u_texture, f_uv);
	vec4 one_color_texture_color = mix(vec4(0.0, 0.0, 0.0, 0.0),
	                                   f_color,
	                                   texture_color.a == 1.0);
	vec4 one_color_flipped_texture_color = mix(vec4(0.0, 0.0, 0.0, 0.0),
	                                           f_color,
	                                           texture_color.a != 1.0);
	vec4 result = mix(texture_color, one_color_texture_color, is_color);
	result = mix(result, one_color_flipped_texture_color, f_flip_color);
	result = mix(f_color, result, is_texture_draw);

	out_color = result;
}
