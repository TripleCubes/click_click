attribute vec2 v_pos;

uniform vec2 u_pos;
uniform vec2 u_sz;

void main() {
	vec2 pos = vec2(u_pos.x + v_pos.x * u_sz.x, u_pos.y + v_pos.y * u_sz.y);
	gl_Position = vec4(pos, 0.0, 1.0);
}
