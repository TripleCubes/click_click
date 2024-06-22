attribute vec2 v_pos;
attribute vec2 v_uv;
attribute vec4 v_color;
attribute float v_flip_color;

varying vec2 f_uv;
varying vec4 f_color;
varying float f_flip_color;

void main() {
	f_uv = vec2(v_uv.x, 1.0 - v_uv.y);
	f_color = v_color;
	f_flip_color = v_flip_color;
	gl_Position = vec4(v_pos, 0.0, 1.0);
}
