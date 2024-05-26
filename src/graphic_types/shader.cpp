#include "shader.h"

#include "graphic_types.h"
#include "../file/file.h"
#include "../types/color.h"
#include "../types/vec2.h"

#include <iostream>

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#else
#include <GLES2/gl2.h>
#endif

namespace {

bool compile_shader(unsigned int &shader, const std::string &path,
bool is_fragment) {
	std::string shader_code;
	if (!file_to_str(shader_code, path)) {
		return false;
	}

	const char *shader_code_c_str = shader_code.c_str();

	shader
		= glCreateShader(is_fragment? GL_FRAGMENT_SHADER: GL_VERTEX_SHADER);
	glShaderSource(shader, 1, &shader_code_c_str, NULL);
	glCompileShader(shader);
	
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char info_log[512];
		glGetShaderInfoLog(shader, 512, NULL, info_log);
		std::cout << "shader compile error" << std::endl;
		std::cout << path << std::endl;
		std::cout << info_log << std::endl;
		return false;
	}

	return true;
}

bool link_shader_program(unsigned int shader_id,
const std::string &vertex_path, const std::string &fragment_path) {
	unsigned int vertex_shader = 0;
	unsigned int fragment_shader = 0;
	if (!compile_shader(vertex_shader, vertex_path, false)) {
		return false;
	}
	if (!compile_shader(fragment_shader, fragment_path, true)) {
		return false;
	}

	glAttachShader(shader_id, vertex_shader);
	glAttachShader(shader_id, fragment_shader);
	glLinkProgram(shader_id);

	int success;
	glGetProgramiv(shader_id, GL_LINK_STATUS, &success);
	if (!success) {
		char info_log[512];
		glGetProgramInfoLog(shader_id, 512, NULL, info_log);
		std::cout << "shader linking error" << std::endl;
		std::cout << vertex_path << std::endl;
		std::cout << fragment_path << std::endl;
		std::cout << info_log << std::endl;
		return false;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return true;
}

}

bool shader_new(GraphicStuff &graphic_stuff, const std::string &path) {
	Shader new_shader;
	graphic_stuff.shader_list.push_back(new_shader);
	int shader_list_sz = (int)graphic_stuff.shader_list.size();

	Shader &shader = graphic_stuff.shader_list[shader_list_sz - 1];
	shader.id = glCreateProgram();
	if (!link_shader_program(shader.id, path + "_v.glsl", path + "_f.glsl")) {
		return false;
	}

	return true;
}

void use_shader(const GraphicStuff &graphic_stuff, int index) {
	glUseProgram(graphic_stuff.shader_list[index].id);
}

bool set_uniform_color(const GraphicStuff &graphic_stuff, int index,
const std::string &uniform_name, Color color) {
	unsigned int shader_id = graphic_stuff.shader_list[index].id;
	GLint location = glGetUniformLocation(shader_id, uniform_name.c_str());
	if (location == -1) {
		std::cout << "cant find " << uniform_name << " uniform" << std::endl;
		return false;
	}

	glUniform4f(location, color.r, color.g, color.b, color.a);

	return true;
}

bool set_uniform_vec2(const GraphicStuff &graphic_stuff, int index,
const std::string &uniform_name, Vec2 vec) {
	unsigned int shader_id = graphic_stuff.shader_list[index].id;
	GLint location = glGetUniformLocation(shader_id, uniform_name.c_str());
	if (location == -1) {
		std::cout << "cant find " << uniform_name << " uniform" << std::endl;
		return false;
	}

	glUniform2f(location, vec.x, vec.y);

	return true;
}

bool set_uniform_int(const GraphicStuff &graphic_stuff, int index,
const std::string &uniform_name, int n) {
	unsigned int shader_id = graphic_stuff.shader_list[index].id;
	GLint location = glGetUniformLocation(shader_id, uniform_name.c_str());
	if (location == -1) {
		std::cout << "cant find " << uniform_name << " uniform" << std::endl;
		return false;
	}

	glUniform1i(location, n);

	return true;
}

bool set_uniform_float(const GraphicStuff &graphic_stuff, int index,
const std::string &uniform_name, float f) {
	unsigned int shader_id = graphic_stuff.shader_list[index].id;
	GLint location = glGetUniformLocation(shader_id, uniform_name.c_str());
	if (location == -1) {
		std::cout << "cant find " << uniform_name << " uniform" << std::endl;
		return false;
	}

	glUniform1f(location, f);

	return true;
}

bool set_uniform_bool(const GraphicStuff &graphic_stuff, int index,
const std::string &uniform_name, bool b) {
	unsigned int shader_id = graphic_stuff.shader_list[index].id;
	GLint location = glGetUniformLocation(shader_id, uniform_name.c_str());
	if (location == -1) {
		std::cout << "cant find " << uniform_name << " uniform" << std::endl;
		return false;
	}

	glUniform1i(location, b);

	return true;
}

bool set_uniform_texture(const GraphicStuff &graphic_stuff, int index,
const std::string &uniform_name, int texture_uniform_index,
unsigned int texture_id) {
	unsigned int shader_id = graphic_stuff.shader_list[index].id;
	GLint location = glGetUniformLocation(shader_id, uniform_name.c_str());
	if (location == -1) {
		std::cout << "cant find " << uniform_name << " uniform" << std::endl;
		return false;
	}

	glUniform1i(location, texture_uniform_index);
	glActiveTexture(GL_TEXTURE0 + texture_uniform_index);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	return true;
}
