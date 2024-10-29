#ifndef FILE__FILE_H
#define FILE__FILE_H

#include <string>

bool file_to_str(std::string &result, const std::string &path);
void write_file(const std::string &file_path, const std::string &str);

#ifdef __EMSCRIPTEN__
bool web_download_file(const std::string &file_path,
	const std::string &download_name);
bool web_download_bin_file(const std::string &file_path,
	const std::string &download_name);
#endif

#endif
