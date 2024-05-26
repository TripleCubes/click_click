#ifndef GRAPHIC_TYPES__SHADER_H
#define GRAPHIC_TYPES__SHADER_H

#include <string>

struct GraphicStuff;
struct Color;
struct Vec2;

bool shader_new(GraphicStuff &graphic_stuff, const std::string &path);
void use_shader(const GraphicStuff &graphic_stuff, int index);

bool set_uniform_color(const GraphicStuff &graphic_stuff, int index,
	const std::string &uniform_name, Color color);
bool set_uniform_vec2(const GraphicStuff &graphic_stuff, int index,
	const std::string &uniform_name, Vec2 vec);
bool set_uniform_int(const GraphicStuff &graphic_stuff, int index,
	const std::string &uniform_name, int n);
bool set_uniform_float(const GraphicStuff &graphic_stuff, int index,
	const std::string &uniform_name, float f);
bool set_uniform_bool(const GraphicStuff &graphic_stuff, int index,
	const std::string &uniform_name, bool b);
bool set_uniform_texture(const GraphicStuff &graphic_stuff, int index,
	const std::string &uniform_name, int texture_uniform_index,
	unsigned int texture_id);

#endif
