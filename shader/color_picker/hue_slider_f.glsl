#version 330 core

in vec2 f_uv;
out vec4 out_color;

float clampf(float f, float min, float max) {
	if (f < min) { return min; }
	else if (f > max) { return max; }
	return f;
}

vec4 hue_to_rgb(float hue) {
	vec4 color;

	if (hue >= 0 && hue < 1/6.0f) {
		color.r = 1;
		color.g = hue*6;
		color.b = 0;
	}
	else if (hue >= 1/6.0f && hue < 2/6.0f) {
		color.r = 1 - ((hue - 1/6.0f) * 6);
		color.g = 1;
		color.b = 0;
	}
	else if (hue >= 2/6.0f && hue < 3/6.0f) {
		color.r = 0;
		color.g = 1;
		color.b = (hue - 2/6.0f) * 6;
	}
	else if (hue >= 3/6.0f && hue < 4/6.0f) {
		color.r = 0;
		color.g = 1 - ((hue - 3/6.0f) * 6);
		color.b = 1;
	}
	else if (hue >= 4/6.0f && hue < 5/6.0f) {
		color.r = (hue - 4/6.0f) * 6;
		color.g = 0;
		color.b = 1;
	}
	else if (hue >= 5/6.0f && hue <= 1) {
		color.r = 1;
		color.g = 0;
		color.b = 1 - ((hue - 5/6.0f) * 6);
	}

	color.r = clampf(color.r, 0, 1);
	color.g = clampf(color.g, 0, 1);
	color.b = clampf(color.b, 0, 1);
	color.a = 1;

	return color;
}

void main() {
	out_color = hue_to_rgb(f_uv.x);
}
