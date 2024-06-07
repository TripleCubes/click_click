precision mediump float;

varying vec2 f_uv;

float clampf(float f, float min, float max) {
	if (f < min) { return min; }
	else if (f > max) { return max; }
	return f;
}

vec4 hue_to_rgb(float hue) {
	vec4 color;

	if (hue >= 0.0 && hue < 1.0/6.0) {
		color.r = 1.0;
		color.g = hue*6.0;
		color.b = 0.0;
	}
	else if (hue >= 1.0/6.0 && hue < 2.0/6.0) {
		color.r = 1.0 - ((hue - 1.0/6.0) * 6.0);
		color.g = 1.0;
		color.b = 0.0;
	}
	else if (hue >= 2.0/6.0 && hue < 3.0/6.0) {
		color.r = 0.0;
		color.g = 1.0;
		color.b = (hue - 2.0/6.0) * 6.0;
	}
	else if (hue >= 3.0/6.0 && hue < 4.0/6.0) {
		color.r = 0.0;
		color.g = 1.0 - ((hue - 3.0/6.0) * 6.0);
		color.b = 1.0;
	}
	else if (hue >= 4.0/6.0 && hue < 5.0/6.0) {
		color.r = (hue - 4.0/6.0) * 6.0;
		color.g = 0.0;
		color.b = 1.0;
	}
	else if (hue >= 5.0/6.0 && hue <= 1.0) {
		color.r = 1.0;
		color.g = 0.0;
		color.b = 1.0 - ((hue - 5.0/6.0) * 6.0);
	}

	color.r = clampf(color.r, 0.0, 1.0);
	color.g = clampf(color.g, 0.0, 1.0);
	color.b = clampf(color.b, 0.0, 1.0);
	color.a = 1.0;

	return color;
}

void main() {
	gl_FragColor = hue_to_rgb(f_uv.x);
}
