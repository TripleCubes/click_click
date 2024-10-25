#include "file.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <iostream>
#include <fstream>
#include <sstream>

bool file_to_str(std::string &result, const std::string &path) {
	std::ifstream i(path);
	if (!i.good()) {
		std::cout << path << " not found" << std::endl;
		return false;
	}

	std::stringstream ss;
	ss << i.rdbuf();
	result = ss.str();
	
	return true;
}

void write_file(const std::string &file_path, const std::string &str) {
	std::ofstream ofs;
	ofs.open(file_path);
	ofs << str;
	ofs.close();
}

#ifdef __EMSCRIPTEN__
bool web_download_file(const std::string &file_name) {
	std::string file_content;
	if (!file_to_str(file_content, "./data/" + file_name)) {
		return false;
	}
	
	EM_ASM({
		let a = document.createElement('a');
		a.href = 'data:text/plain;charset=utf-8,'
			+ encodeURIComponent(UTF8ToString($0));

		a.download = UTF8ToString($1);
		a.display = 'none';
		document.body.appendChild(a);
		a.click();
		document.body.removeChild(a);
	}, file_content.c_str(), file_name.c_str());

	return true;
}
#endif
