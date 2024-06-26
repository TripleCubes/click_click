#include "color.h"

#include "../basic_math.h"

namespace {

Color cap(Color color) {
	if (color.r < 0) {
		color.r = 0;
	}
	if (color.r > 1) {
		color.r = 1;
	}

	if (color.g < 0) {
		color.g = 0;
	}
	if (color.g > 1) {
		color.g = 1;
	}
	
	if (color.b < 0) {
		color.b = 0;
	}
	if (color.b > 1) {
		color.b = 1;
	}

	if (color.a < 0) {
		color.a = 0;
	}
	if (color.a > 1) {
		color.a = 1;
	}
	return color;
}

float hue_rgb_to_hue(Color hue_rgb) {
	float result = 0;

	if (hue_rgb.r == 1 && hue_rgb.b == 0) {
		result = hue_rgb.g / 6;
	}
	if (hue_rgb.g == 1 && hue_rgb.b == 0) {
		result = (1 - hue_rgb.r) / 6 + 1/6.0f;
	}
	if (hue_rgb.r == 0 && hue_rgb.g == 1) {
		result = hue_rgb.b / 6 + 2/6.0f;
	}
	if (hue_rgb.r == 0 && hue_rgb.b == 1) {
		result = (1 - hue_rgb.g) / 6 + 3/6.0f;
	}
	if (hue_rgb.g == 0 && hue_rgb.b == 1) {
		result = hue_rgb.r / 6 + 4/6.0f;
	}
	if (hue_rgb.r == 1 && hue_rgb.g == 0) {
		result = (1 - hue_rgb.b) / 6 + 5/6.0f;
	}
	
	result = clampf(result, 0, 1);
	return result;
}

Color saturation_scale(Color hue_rgb, float sat) {
	Color color = hue_rgb;

	color.r = 1 - ((1 - color.r) * sat);
	color.g = 1 - ((1 - color.g) * sat);
	color.b = 1 - ((1 - color.b) * sat);

	color.r = clampf(color.r, 0, 1);
	color.g = clampf(color.g, 0, 1);
	color.b = clampf(color.b, 0, 1);
	color.a = 1;

	return color;
}

Color value_scale(Color hue_sat_rgb, float value) {
	Color color = hue_sat_rgb;
	
	color.r = color.r * value;
	color.g = color.g * value;
	color.b = color.b * value;

	color.r = clampf(color.r, 0, 1);
	color.g = clampf(color.g, 0, 1);
	color.b = clampf(color.b, 0, 1);
	color.a = 1;

	return color;
}

}

Color color_new(float r, float g, float b, float a) {
	Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	return color;
}

Color color_add(Color color_a, Color color_b) {
	Color result;
	result.r = color_a.r + color_b.r;
	result.g = color_a.g + color_b.g;
	result.b = color_a.b + color_b.b;
	result.a = color_a.a + color_b.a;
	result = cap(result);
	return result;
}

Color color_sub(Color color_a, Color color_b) {
	Color result;
	result.r = color_a.r - color_b.r;
	result.g = color_a.g - color_b.g;
	result.b = color_a.b - color_b.b;
	result.a = color_a.a - color_b.a;
	result = cap(result);
	return result;
}

Color hue_to_rgb(float hue) {
	Color color;

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

Color hsv_to_rgb(Color hsv) {
	Color result;
	
	result = hue_to_rgb(hsv.r);
	result = saturation_scale(result, hsv.g);
	result = value_scale(result, hsv.b);

	return result;
}

Color rgb_to_hsv(Color rgb) {
	Color hue_sat_rgb;

	float value = max3f(rgb.r, rgb.g, rgb.b);
	hue_sat_rgb.r = rgb.r / value;
	hue_sat_rgb.g = rgb.g / value;
	hue_sat_rgb.b = rgb.b / value;


	Color hue_rgb;
	float saturation = max3f(
		1 - hue_sat_rgb.r,
		1 - hue_sat_rgb.g,
		1 - hue_sat_rgb.b
	);
	hue_rgb.r = 1 - ((1 - hue_sat_rgb.r) / saturation);
	hue_rgb.g = 1 - ((1 - hue_sat_rgb.g) / saturation);
	hue_rgb.b = 1 - ((1 - hue_sat_rgb.b) / saturation);


	float hue = hue_rgb_to_hue(hue_rgb);

	Color result = color_new(hue, saturation, value, 1);
	result.a = 1;
	result = cap(result);

	return result;
}
