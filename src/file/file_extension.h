#ifndef FILE__FILE_EXTENSION_H
#define FILE__FILE_EXTENSION_H

#include <string>

bool is_dot_extension(const std::string &str, const std::string &ex);

bool is_dot_click(const std::string &str);

bool is_dot_png(const std::string &str);

void rm_extension(std::string &str);

#endif
