#ifndef FILE__FILE_H
#define FILE__FILE_H

#include <string>

bool file_to_str(std::string &result, const std::string &path);
void write_file(const std::string &file_path, const std::string &str);

#endif
