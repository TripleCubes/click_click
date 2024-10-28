#include "file_extension.h"

#include "../consts.h"

bool is_dot_extension(const std::string &str, const std::string &ex) {
	if (str.length() <= ex.length()) {
		return false;
	}

	for (int i = 0; i < (int)ex.length(); i++) {
		if (str[str.length() - ex.length() + i] != ex[i]) {
			return false;
		}
	}

	return true;
}

bool is_dot_click(const std::string &str) {
	return is_dot_extension(str, DOT_CLICK);
}

bool is_dot_png(const std::string &str) {
	return is_dot_extension(str, DOT_PNG);
}

void rm_extension(std::string &str) {
	for (int i = (int)str.length() - 1; i >= 0; i--) {
		if (str[i] == '.') {
			str.pop_back();
			break;
		}

		str.pop_back();
	}
}
