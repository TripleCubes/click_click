#include "open_project.h"

#include "file.h"

#include <iostream>

namespace {

const std::vector<char> WHITE_SPACE_LIST = {
	' ', '\n', '\t', '\r',
};
const std::vector<std::string> FIELD_LIST = {
	"SZ", "PALLETE", "LAYER", "PXSCALE",
};

const int FIELD = 0;
const int NUM = 1;
const int STR = 2;
const int TEXT = 3;

struct Word {
	int type = FIELD;
	std::string str;
};

std::vector<std::string> layer_data;

std::string left_pad(int a, int n) {
	std::string s = std::to_string(a);
	std::string pad = "";
	int diff = n - s.size();
	for (int i = 0; i < diff; i++) {
		pad += ' ';
	}
	return pad + s;
}

[[maybe_unused]]
std::string word_type_to_str(int type) {
	switch (type) {
		case FIELD: return "FIELD";
		case NUM  : return "NUM  ";
		case STR  : return "STR  ";
		case TEXT : return "TEXT ";
		default   : return "";
	}
}

[[maybe_unused]]
void print_word_list(const std::vector<Word> &word_list) {
	for (int i = 0; i < (int)word_list.size(); i++) {
		const Word &word = word_list[i];
		std::cout << i << ' ' << word_type_to_str(word.type)
			<< ' ' << word.str << std::endl;
	}
}

[[maybe_unused]]
void print_open_project_data(const OpenProjectData &data) {
	std::cout << "sz: " << data.sz.x << " " << data.sz.y << std::endl;
	std::cout << "pos: " << data.pos.x << " " << data.pos.y << std::endl;
	std::cout << "px_scale " << data.px_scale << std::endl;
	std::cout << "pallete_data: " << std::endl;
	for (int i = 0; i < 64; i++) {
		std::cout
			<< left_pad((int)data.pallete_data[i * 4    ], 3) << " "
			<< left_pad((int)data.pallete_data[i * 4 + 1], 3) << " "
			<< left_pad((int)data.pallete_data[i * 4 + 2], 3) << " "
			<< left_pad((int)data.pallete_data[i * 4 + 3], 3) << " ";

		std::cout << ' ';

		if (i % 4 == 3) {
			std::cout << std::endl;
		}
	}

	for (int i = 0; i < (int)data.layer_list.size(); i++) {
		const OpenProjectLayerData &layer = data.layer_list[i];
		std::cout << "layer " << layer.layer_name << std::endl;

		for (int y = 0; y < data.sz.y; y++) {
		for (int x = 0; x < data.sz.x; x++) {
			std::cout << left_pad((int)layer.data[y * data.sz.x + x], 2)
				<< ' ';
		}
			std::cout << std::endl;
		}
	}
}

bool is_white_space(char c) {
	for (int i = 0; i < (int)WHITE_SPACE_LIST.size(); i++) {
		if (c == WHITE_SPACE_LIST[i]) {
			return true;
		}
	}

	return false;
}

bool is_field(const std::string &str) {
	for (int i = 0; i < (int)FIELD_LIST.size(); i++) {
		if (FIELD_LIST[i] == str) {
			return true;
		}
	}

	return false;
}

int get_word_type(const std::string &word_str) {
	if (word_str[0] == '"') {
		return STR;
	}

	if (is_field(word_str)) {
		return FIELD;
	}

	for (int i = 0; i < (int)word_str.length(); i++) {
		char c = word_str[i];

		if (c < '0' || c > '9') {
			return TEXT;
		}
	}

	return NUM;
}

void to_word_list(std::vector<Word> &word_list, const std::string &file_str) {
	auto add_word = [&word_list](const std::string &word_str) {
		int word_type = get_word_type(word_str);
		Word word;
		word.type = word_type;
		word.str = word_str;
		if (word_type == STR) {
			word.str.erase(0, 1);
			word.str.pop_back();
		}
		word_list.push_back(word);
	};

	std::string word_str;
	bool in_quote = false;

	for (int i = 0; i < (int)file_str.length(); i++) {
		char c = file_str[i];
		char next_c = ' ';
		if (i + 1 < (int)file_str.length()) {
			next_c = file_str[i + 1];
		}

		bool c_ws = is_white_space(c);
		bool next_ws = is_white_space(next_c);
		if (!in_quote) {
			if (c == '"') {
				if (word_str != "") {
					add_word(word_str);
					word_str = "";
				}
				in_quote = true;
			}

			if (!c_ws && next_ws) {
				word_str += c;
				add_word(word_str);
				word_str = "";
			}
			else if (!c_ws && !next_ws) {
				word_str += c;
			}
		}
		else {
			if (c == '"') {
				in_quote = false;
				word_str += c;
				add_word(word_str);
				word_str = "";
			}
			else {
				word_str += c;
			}
		}
	}

	if (word_str != "") {
		add_word(word_str);
	}
}

void err(const std::string &str) {
	std::cout << "file reading error: " << str << std::endl;
}

bool update_layer_data(std::vector<std::string> &layer_data,
const std::vector<Word> &word_list, int word_list_index) {
	int count = 0;

	for (int i = word_list_index + 3; i < (int)word_list.size(); i++) {
		const Word &word = word_list[i];

		if (word.type == FIELD) {
			break;
		}

		count++;

		if (count < 3) {
			continue;
		}

		if (word.str == "y") {
			if (i + 1 >= (int)word_list.size()) {
				err("update_layer_data(): i + 1 > word_list.size()");
				return false;
			}
			layer_data.push_back(word_list[i + 1].str);
			count = -1;
		}
		else {
			layer_data.push_back("");
			count = 0;
		}
	}

	return true;
}

int div_1(int a, int b) {
	if (a % b == 0) {
		return a / b;
	}
	else {
		return a / b + 1;
	}
}

int mod_1(int a, int b) {
	if (a % b == 0) {
		return b;
	}
	return a % b;
}

unsigned char to_pallete_index(char c) {
	return (unsigned char)c - 48;
}

void copy_data(std::vector<unsigned char> &data, const std::string &str,
int in_x, int in_y, int w, int h, int canvas_w) {
	for (int y = in_y; y < in_y + h; y++) {
	for (int x = in_x; x < in_x + w; x++) {
		int str_i = (y % 8) * w + (x % 8);
		int data_i = y * canvas_w + x;

		data[data_i] = to_pallete_index(str[str_i]);
	}
	}
}

bool word_list_to_data(OpenProjectData &data,
const std::vector<Word> &word_list) {
	const int BEFORE_PALLETE_DATA = 11;

	if (word_list.size() < BEFORE_PALLETE_DATA + 64*3) {
		err("file too short");
		return false;
	}

	if (word_list[1].type != NUM) {
		err("field VER is not number");
		return false;
	}
	if (word_list[3].type != NUM) {
		err("field SZ x is not number");
		return false;
	}
	if (word_list[4].type != NUM) {
		err("field SZ y is not number");
		return false;
	}
	if (word_list[6].type != NUM) {
		err("field POS x is not number");
		return false;
	}
	if (word_list[7].type != NUM) {
		err("field POS y is not number");
		return false;
	}
	if (word_list[9].type != NUM) {
		err("field PXSCALE is not number");
		return false;
	}
	data.ver = std::stoi(word_list[1].str);
	data.sz.x = std::stoi(word_list[3].str);
	data.sz.y = std::stoi(word_list[4].str);
	data.pos.x = std::stof(word_list[6].str);
	data.pos.y = std::stof(word_list[7].str);
	data.px_scale = std::stoi(word_list[9].str);

	int pallete_data_i = 0;
	for (int i = BEFORE_PALLETE_DATA; i < BEFORE_PALLETE_DATA + 64*3; i += 3) {
		if (word_list[i].type != NUM) {
			err("pallete data not number");
			return false;
		}
		data.pallete_data[pallete_data_i]
			= (unsigned char)std::stoi(word_list[i    ].str);
		data.pallete_data[pallete_data_i + 1]
			= (unsigned char)std::stoi(word_list[i + 1].str);
		data.pallete_data[pallete_data_i + 2]
			= (unsigned char)std::stoi(word_list[i + 2].str);
		data.pallete_data[pallete_data_i + 3] = 255;

		pallete_data_i += 4;
	}
	
	for (int i = 0; i < 4; i++) {
		data.pallete_data[i] = 0;
	}

	int chunk_num_x = div_1(data.sz.x, 8);
	int chunk_num_y = div_1(data.sz.y, 8);
	int chunk_num = chunk_num_x * chunk_num_y;
	int border_chunk_w = mod_1(data.sz.x, 8);
	int border_chunk_h = mod_1(data.sz.y, 8);
	for (int i = BEFORE_PALLETE_DATA + 64*3; i < (int)word_list.size(); i++) {
		layer_data.clear();
		const Word &word = word_list[i];

		if (word.type != FIELD || word.str != "LAYER") {
			continue;
		}

		if (i + 1 >= (int)word_list.size()) {
			err("word_list_to_data(): i + 1 < word_list.size()");
			return false;
		}

		OpenProjectLayerData new_op_layer_data;
		data.layer_list.push_back(new_op_layer_data);
		OpenProjectLayerData &op_layer_data = data.layer_list.back();
		op_layer_data.layer_name = word_list[i + 1].str;
		op_layer_data.data.resize(data.sz.x * data.sz.y, 0);

		if (!update_layer_data(layer_data, word_list, i)) {
			return false;
		}

		if ((int)layer_data.size() != chunk_num) {
			err("number of chunks not match with canvas size");
			return false;
		}

		for (int y = 0; y < chunk_num_y; y++) {
		for (int x = 0; x < chunk_num_x; x++) {
			int index = y * chunk_num_x + x;
			if (layer_data[index].length() == 0) {
				continue;
			}

			if (y == chunk_num_y - 1 && x == chunk_num_x - 1) {
				if ((int)layer_data[index].length()
				!= border_chunk_w * border_chunk_h) {
					err("chunk size ("
						+ std::to_string(layer_data[index].length())
						+ ") not match border_chunk_wh("
						+ std::to_string(border_chunk_w) + " * "
						+ std::to_string(border_chunk_h)
						+ ")");
					return false;
				}
				copy_data(
					op_layer_data.data, layer_data[index],
					x * 8, y * 8, border_chunk_w, border_chunk_h, data.sz.x
				);
			}
			else if (y == chunk_num_y - 1) {
				if ((int)layer_data[index].length() != border_chunk_h * 8) {
					err("chunk size ("
						+ std::to_string(layer_data[index].length())
						+ ") not match border_chunk_h ("
						+ std::to_string(border_chunk_h)
						+ ") * 8");
					return false;
				}
				copy_data(
					op_layer_data.data, layer_data[index],
					x * 8, y * 8, 8, border_chunk_h, data.sz.x
				);
			}
			else if (x == chunk_num_x - 1) {
				if ((int)layer_data[index].length() != border_chunk_w * 8) {
					err("chunk size ("
						+ std::to_string(layer_data[index].length())
						+ ") not match border_chunk_w ("
						+ std::to_string(border_chunk_w)
						+ ") * 8");
					return false;
				}
				copy_data(
					op_layer_data.data, layer_data[index],
					x * 8, y * 8, border_chunk_w, 8, data.sz.x
				);
			}
			else {
				if ((int)layer_data[index].length() != 8 * 8) {
					err("chunk size ("
						+ std::to_string(layer_data[index].length())
						+ ") not match 8 * 8");
					return false;
				}
				copy_data(
					op_layer_data.data, layer_data[index],
					x * 8, y * 8, 8, 8, data.sz.x
				);
			}
		}
		}
	}

//	print_word_list(word_list);
//	print_open_project_data(data);

	return true;
}

}

bool file_to_project_data(OpenProjectData &data, const std::string &path) {
	std::string file_str;
	if (!file_to_str(file_str, path)) {
		return false;
	}

	std::vector<Word> word_list;
	to_word_list(word_list, file_str);

	if (!word_list_to_data(data, word_list)) {
		return false;
	}

	return true;
}
