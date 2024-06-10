attribute vec2 v_pos;

varying vec2 f_uv;

uniform bool u_flip;
uniform vec2 u_from_pos;
uniform vec2 u_from_sz;
uniform vec2 u_pos;
uniform vec2 u_sz;

void main() {
	if (!u_flip) {
		f_uv = vec2(
			u_from_pos.x + v_pos.x * u_from_sz.x,
			u_from_pos.y + v_pos.y * u_from_sz.y
		);
	}
	else {
		f_uv = vec2(
			u_from_pos.x + v_pos.x * u_from_sz.x,
			u_from_pos.y + (1.0 - v_pos.y) * u_from_sz.y
		);
	}
	vec2 pos = vec2(u_pos.x + v_pos.x * u_sz.x, u_pos.y + v_pos.y * u_sz.y);
	gl_Position = vec4(pos, 0.0, 1.0);
}
