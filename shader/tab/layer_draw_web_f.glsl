precision mediump float;

varying vec2 f_uv;

uniform sampler2D u_draw_texture;
uniform sampler2D u_pallete_texture;

int mod(int a, int b) {
	return a - (a / b * b);
}

vec4 get_color(int index) {
	vec2 pos = vec2(mod(index, 16), index / 16);
	pos.x = pos.x / 16.0 + 1.0/16.0/2.0;
	pos.y = pos.y / 16.0 + 1.0/16.0/2.0;
	
	return texture2D(u_pallete_texture, pos);
}

void main() {
	int index = int(texture2D(u_draw_texture, f_uv).a * 255.0);

	gl_FragColor = get_color(index);
}
