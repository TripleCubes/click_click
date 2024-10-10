#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <vector>

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#else
#include <GLES2/gl2.h>
#endif

struct States;
struct Settings;
struct GraphicStuff;
struct GameTime;
struct Input;
struct AppUI;

void update(
	States &states,
	Settings &settings,
	GraphicStuff &gs,
	const GameTime &game_time,
	const Input &input,
	AppUI &app_ui
	#ifndef __EMSCRIPTEN__
	,GLFWwindow *glfw_window
	#endif
);
void draw(
	const States &states,
	const Settings &settings,
	GraphicStuff &gs,
	const GameTime &game_time,
	const Input &input,
	const AppUI &app_ui
);

#endif
