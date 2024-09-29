#ifndef CONSTS_H
#define CONSTS_H

#include "types/color.h"
#include "types/vec2.h"
#include "types/vec2i.h"

const Color BTN_TEXTAREA_COLOR = color_new(0.2, 0.2, 0.2, 1);
const Color KEY_HINT_COLOR = color_new(0.6, 0.6, 0.6, 1);
const Color WARNING_TEXT_COLOR = color_new(0.85, 0.41, 0.41, 1);

const float SIDE_BAR_W = 100;
const float TOP_BAR_H = 12 + 5 + 4;

const Vec2i DEFL_CANVAS_SZ = vec2i_new(64, 64);

#endif
