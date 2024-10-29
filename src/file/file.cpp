#include "file.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <iostream>
#include <fstream>
#include <sstream>

#include "../consts.h"

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
bool web_download_file(const std::string &file_path,
const std::string &download_name) {
	std::string file_content;
	if (!file_to_str(file_content, file_path)) {
		return false;
	}
	
	EM_ASM({
		let a = document.createElement('a');
		a.href = 'data:text/plain;charset=utf-8,'
			+ encodeURIComponent(UTF8ToString($0));

		a.download = UTF8ToString($1);
		a.style.display = 'none';
		document.body.appendChild(a);
		a.click();
		document.body.removeChild(a);
	}, file_content.c_str(), download_name.c_str());

	return true;
}

bool web_download_bin_file(const std::string &file_path,
const std::string &download_name) {
	EM_ASM({
		let u8arr = FS.readFile(UTF8ToString($0), { encoding: 'binary' });
		let blob = new Blob([u8arr]);
		let a = document.createElement('a');
		a.href = window.URL.createObjectURL(blob);
		a.download = UTF8ToString($1);
		a.style.display = 'none';
		document.body.appendChild(a);
		a.click();
		document.body.removeChild(a);
	}, file_path.c_str(), download_name.c_str());

	return true;
}
#endif
