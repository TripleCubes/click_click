precision mediump float;
varying vec2 f_uv;
varying vec4 f_color;
varying float f_flip_color;

uniform sampler2D u_texture;

void main() {
	float is_texture_draw = float(f_uv.x != -1.0);
	float is_color = float(f_color.a != 0.0);

	vec4 texture_color = texture2D(u_texture, f_uv);
	vec4 one_color_texture_color = mix(vec4(0.0, 0.0, 0.0, 0.0),
	                                   f_color,
	                                   float(texture_color.a == 1.0));
	vec4 one_color_flipped_texture_color = mix(vec4(0.0, 0.0, 0.0, 0.0),
	                                           f_color,
	                                           float(texture_color.a != 1.0));
	vec4 result = mix(texture_color, one_color_texture_color, is_color);
	result = mix(result, one_color_flipped_texture_color, f_flip_color);
	result = mix(f_color, result, is_texture_draw);

	gl_FragColor = result;
}
