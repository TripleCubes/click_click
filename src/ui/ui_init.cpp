#include "ui_init.h"

#include <vector>
#include "btn.h"

void ui_init(std::vector<Btn> &btn_list) {
	btn_new(btn_list,
		vec2_new(200, 50),
		vec2_new(50, 30),
		color_new(183/255.0f, 1, 193/255.0f, 1),
		"",
		true);
}
