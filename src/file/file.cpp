#include "file.h"

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
