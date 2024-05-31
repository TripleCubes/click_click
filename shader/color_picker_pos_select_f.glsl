#version 330 core

in vec2 f_uv;
out vec4 out_color;

uniform vec4 u_hue_rgb;

float clampf(float f, float min, float max) {
	if (f < min) { return min; }
	else if (f > max) { return max; }
	return f;
}

vec4 saturation_scale(vec4 hue_rgb, float sat) {
	vec4 color = hue_rgb;

	color.r = 1 - ((1 - color.r) * sat);
	color.g = 1 - ((1 - color.g) * sat);
	color.b = 1 - ((1 - color.b) * sat);

	color.r = clampf(color.r, 0, 1);
	color.g = clampf(color.g, 0, 1);
	color.b = clampf(color.b, 0, 1);
	color.a = 1;

	return color;
}

vec4 value_scale(vec4 hue_sat_rgb, float value) {
	vec4 color = hue_sat_rgb;
	
	color.r = color.r * value;
	color.g = color.g * value;
	color.b = color.b * value;

	color.r = clampf(color.r, 0, 1);
	color.g = clampf(color.g, 0, 1);
	color.b = clampf(color.b, 0, 1);
	color.a = 1;

	return color;
}

void main() {
	vec4 result = u_hue_rgb;
	result = saturation_scale(result, f_uv.x);
	result = value_scale(result, f_uv.y);
	out_color = result;
}
