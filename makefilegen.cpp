#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <algorithm>

struct Config {
	std::string compiler;
	std::string link_flags;
	std::string compile_flags;
	std::string obj_path;
	std::string bin_path;
	std::string src_path;
	std::string output_name;
	std::string lib_path;
	bool is_cpp = false;
	bool is_lib = false;
	std::vector<std::string> file_list;
};

void write_file(const std::string &file_path, const std::string &str) {
	std::ofstream ofs;
	ofs.open(file_path);
	ofs << str;
	ofs.close();
}

void create_build_dir() {
	namespace fs = std::filesystem;
	fs::create_directories("./build/native/bin/");
	fs::create_directories("./build/native/obj/");
	fs::create_directories("./build/web/bin/");
	fs::create_directories("./build/web/obj/");
	fs::create_directories("./build/glfw/bin/");
	fs::create_directories("./build/glfw/obj/");
	fs::create_directories("./build/glad/bin/");
	fs::create_directories("./build/glad/obj/");
}

std::string to_underscore_path(const std::string &path) {
	std::string path_underscore = path;
	for (int i = (int)path_underscore.length() - 1; i >= 0; i--) {
		if (path_underscore[i] == '/') {
			path_underscore.insert(i, "_");
		}
	}
	std::replace(path_underscore.begin(), path_underscore.end(), '/', '_');
	return path_underscore;
}

void get_link_rule(std::string &result, const Config &config) {
	std::string obj_file_list;
	for (int i = 0; i < (int)config.file_list.size(); i++) {
		const std::string &path = config.file_list[i];
		obj_file_list += config.obj_path + to_underscore_path(path) + ".o";
		if (i != (int)config.file_list.size() - 1) {
			obj_file_list += ' ';
		}
	}
	
	if (config.is_lib) {
		result += config.bin_path + config.output_name + ".a: ";
		result += obj_file_list + '\n';
		result += "\tar rvs " + config.bin_path + config.output_name + ".a ";
		result += obj_file_list + '\n';
	}
	else {
		result += "all: " + config.lib_path + ' ';
		result += obj_file_list + '\n';
		result += "\t" + config.compiler + ' '
			+ "-o " + config.bin_path + config.output_name + ' '
			+ obj_file_list + ' ' + config.lib_path + ' '
			+ config.link_flags + '\n';
	}
}

void get_include_rule(std::string &result, const Config &config) {
	result += "-include ";
	for (int i = 0; i < (int)config.file_list.size(); i++) {
		const std::string &path = config.file_list[i];
		result += config.obj_path + to_underscore_path(path) + ".d";
		if (i != (int)config.file_list.size() - 1) {
			result += ' ';
		}
	}
	result += '\n';
}

void get_compile_rule(std::string &result, const Config &config) {
	std::string ext;
	if (config.is_cpp) {
		ext = ".cpp";
	}
	else {
		ext = ".c";
	}

	for (int i = 0; i < (int)config.file_list.size(); i++) {
		const std::string &path = config.file_list[i];

		std::string underscore_path = to_underscore_path(path);

		std::string rule = config.obj_path + underscore_path + ".o: "
			+ config.src_path + path + ext;

		if (config.is_lib) {
			rule += '\n';
		}
		else {
			rule += " makefile\n";
		}

		rule += "\t" + config.compiler + ' '
			+ "-c " + config.src_path + path + ext + ' '
			+ "-o " + config.obj_path + underscore_path + ".o "
			+ config.compile_flags + '\n';

		result += rule;

		if (i != (int)config.file_list.size() - 1) {
			result += '\n';
		}
	}
}

void get_makefile_str(std::string &result, const Config &config) {
	std::string link_rule;
	get_link_rule(link_rule, config);

	std::string include_rule;
	get_include_rule(include_rule, config);

	std::string compile_rule;
	get_compile_rule(compile_rule, config);

	result = link_rule + '\n' + include_rule + '\n' + compile_rule;
}

void get_file_list_no_ext(std::vector<std::string> &file_list,
const std::string &dir_path, const std::string &src_path) {
	namespace fs = std::filesystem;
	for (const auto &f: fs::directory_iterator(dir_path)) {
		if (f.is_directory()) {
			get_file_list_no_ext(file_list, f.path().string(), src_path);
			continue;
		}

		bool found = false;
		bool is_cpp = false;
		int f_path_sz = (int)f.path().string().length();
		if ((int)f.path().string().find(".cpp") == f_path_sz - 4) {
			found = true;
			is_cpp = true;
		}
		else if ((int)f.path().string().find(".c") == f_path_sz - 2) {
			found = true;
			is_cpp = false;
		}

		if (!found) {
			continue;
		}

		std::string replaced = f.path().string();
		std::replace(replaced.begin(), replaced.end(), (char)92, '/');
		replaced.replace(0, (int)src_path.length(), "");
		if (is_cpp) {
			replaced = replaced.substr(0, replaced.length() - 4);
		}
		else {
			replaced = replaced.substr(0, replaced.length() - 2);
		}
		file_list.push_back(replaced);
	}
}

const std::string SRC_PATH = "./src/";
const std::string GLFW_SRC_PATH = "./lib/glfw-3.3.8/src/";
const std::string GLAD_SRC_PATH = "./lib/glad/src/";
const std::string GLFW_INCLUDE_FLAG = "-I./lib/glfw-3.3.8/include/";
const std::string GLAD_INCLUDE_FLAG = "-I./lib/glad/include/";
const std::string STB_INCLUDE_FLAG = "-I./lib/stb/include/";

void makefilegen_glad_glfw(std::string &result) {
	Config glad_config;
	glad_config.compiler = "gcc";
	glad_config.link_flags = "";
	glad_config.compile_flags = "-MMD -MP " + GLAD_INCLUDE_FLAG;
	glad_config.obj_path = "./build/glad/obj/";
	glad_config.bin_path = "./build/glad/bin/";
	glad_config.src_path = GLAD_SRC_PATH;
	glad_config.output_name = "glad";
	glad_config.lib_path = "";
	glad_config.is_cpp = false;
	glad_config.is_lib = true;
	glad_config.file_list = {
		"glad",
	};

	std::string glad_makefile_str;
	get_makefile_str(glad_makefile_str, glad_config);


	std::vector<std::string> glfw_all_plat_file_list = {
		"context",
		"init",
		"input",
		"monitor",
		"vulkan",
		"window",
	};
	std::vector<std::string> glfw_window_plat_file_list {
		"win32_init",
		"win32_joystick",
		"win32_monitor",
		"win32_time",
		"win32_thread",
		"win32_window",
		"wgl_context",
		"egl_context",
		"osmesa_context",
	};

	Config glfw_config;
	glfw_config.compiler = "gcc";
	glfw_config.link_flags = "";
	glfw_config.compile_flags = "-MMD -MP -D_GLFW_WIN32 " + GLFW_INCLUDE_FLAG;
	glfw_config.obj_path = "./build/glfw/obj/";
	glfw_config.bin_path = "./build/glfw/bin/";
	glfw_config.src_path = GLFW_SRC_PATH;
	glfw_config.output_name = "glfw";
	glfw_config.lib_path = "";
	glfw_config.is_cpp = false;
	glfw_config.is_lib = true;
	glfw_config.file_list = glfw_all_plat_file_list;
	glfw_config.file_list.insert(
		glfw_config.file_list.end(),
		glfw_window_plat_file_list.begin(),
		glfw_window_plat_file_list.end()
	);

	std::string glfw_makefile_str;
	get_makefile_str(glfw_makefile_str, glfw_config);

	result = glad_makefile_str + "\n\n" + glfw_makefile_str;
}

void makefilegen_native(std::string &result) {
	std::string glad_glfw_makefile_str;
	makefilegen_glad_glfw(glad_glfw_makefile_str);

	Config native_config;
	native_config.compiler = "g++";
	native_config.link_flags
		= "-Wall -g3 -lgdi32";
	native_config.compile_flags = "-Wall -g3 -MMD -MP "
		+ GLFW_INCLUDE_FLAG + ' ' + GLAD_INCLUDE_FLAG + ' '
		+ STB_INCLUDE_FLAG;
	native_config.obj_path = "./build/native/obj/";
	native_config.bin_path = "./build/native/bin/";
	native_config.src_path = SRC_PATH;
	native_config.output_name = "clickclick";
	native_config.lib_path = "./build/glfw/bin/glfw.a ./build/glad/bin/glad.a";
	native_config.is_cpp = true;
	native_config.is_lib = false;
	get_file_list_no_ext(native_config.file_list, SRC_PATH, SRC_PATH);
	
	std::string native_makefile_str;
	get_makefile_str(native_makefile_str, native_config);

	result = native_makefile_str + "\n\n" + glad_glfw_makefile_str;
}

void makefilegen_linklib(std::string &result) {
	std::string glad_glfw_makefile_str;
	makefilegen_glad_glfw(glad_glfw_makefile_str);

	Config native_config;
	native_config.compiler = "g++";
	native_config.link_flags
		= "-Wall -g3 -lgdi32 -static-libgcc -static-libstdc++";
	native_config.compile_flags = "-Wall -g3 -MMD -MP "
		+ GLFW_INCLUDE_FLAG + ' ' + GLAD_INCLUDE_FLAG + ' '
		+ STB_INCLUDE_FLAG;
	native_config.obj_path = "./build/native/obj/";
	native_config.bin_path = "./build/native/bin/";
	native_config.src_path = SRC_PATH;
	native_config.output_name = "clickclick";
	native_config.lib_path = "./build/glfw/bin/glfw.a ./build/glad/bin/glad.a";
	native_config.is_cpp = true;
	native_config.is_lib = false;
	get_file_list_no_ext(native_config.file_list, SRC_PATH, SRC_PATH);
	
	std::string native_makefile_str;
	get_makefile_str(native_makefile_str, native_config);

	result = native_makefile_str + "\n\n" + glad_glfw_makefile_str;
}

void makefilegen_web(std::string &result) {
	Config web_config;
	web_config.compiler = "em++";
	web_config.link_flags = "-Wall -sWASM=0 -lglfw ";
	web_config.link_flags += "--embed-file shader --embed-file texture ";
	web_config.link_flags += "-lidbfs.js";
	web_config.compile_flags = "-Wall -MMD -MP " + STB_INCLUDE_FLAG;
	web_config.obj_path = "./build/web/obj/";
	web_config.bin_path = "./build/web/bin/";
	web_config.src_path = SRC_PATH;
	web_config.output_name = "clickclick.js";
	web_config.lib_path = "";
	web_config.is_cpp = true;
	web_config.is_lib = false;
	get_file_list_no_ext(web_config.file_list, SRC_PATH, SRC_PATH);
	
	std::string web_makefile_str;
	get_makefile_str(web_makefile_str, web_config);

	result = web_makefile_str;
}

int main(int argc, char *argv[]) {
	bool linklib = false;
	bool web = false;

	if (argc > 2) {
		std::cout << "too much arguments" << std::endl;
		return 0;
	}

	else if (argc == 2) {
		std::string arg_str(argv[1]);

		if (arg_str == "linklib") {
			linklib = true;
		}

		else if (arg_str == "web") {
			web = true;
		}

		else {
			std::cout << "invalid argument" << std::endl;
			std::cout << "arguments:  linklib  web" << std::endl;
			return 0;
		}
	}

	create_build_dir();

	std::string makefile_str;

	if (!linklib && !web) {
		makefilegen_native(makefile_str);
	}
	else if (linklib) {
		makefilegen_linklib(makefile_str);
	}
	else if (web) {
		makefilegen_web(makefile_str);
	}

	write_file("./makefile", makefile_str);

	std::cout << "makefile and build/ directory created" << std::endl;

	return 0;
}
