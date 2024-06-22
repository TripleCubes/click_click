#include "mesh.h"

#include "graphic_types.h"

#include <vector>
#include <array>

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#else
#include <GLES2/gl2.h>
#endif

namespace {
int get_blank_index(const std::vector<Mesh> &list) {
	for (int i = 0; i < (int)list.size(); i++) {
		if (!list[i].running) {
			return i;
		}
	}

	return -1;
}

}

int mesh_new(GraphicStuff &graphic_stuff){
	int index = get_blank_index(graphic_stuff.mesh_list);
	if (index == -1) {
		Mesh new_mesh;
		graphic_stuff.mesh_list.push_back(new_mesh);

		index = (int)graphic_stuff.mesh_list.size() - 1;
	}
	Mesh &mesh = graphic_stuff.mesh_list[index];

	#ifndef __EMSCRIPTEN__
	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);

	glGenBuffers(1, &mesh.vbo);

	#else
	glGenBuffers(1, &mesh.vbo);
	
	#endif

	return index;
}

void mesh_add(GraphicStuff &gs, int index, const std::vector<float> &add) {
	Mesh &mesh = gs.mesh_list[index];
	mesh.verticies.insert(mesh.verticies.end(), add.begin(), add.end());
}

void mesh_add_arr(GraphicStuff &gs, int index,
const std::array<float, VERT_SZ * 6> &add) {
	Mesh &mesh = gs.mesh_list[index];
	mesh.verticies.insert(mesh.verticies.end(), add.begin(), add.end());
}

void mesh_clear(GraphicStuff &gs, int index) {
	gs.mesh_list[index].verticies.clear();
}

void mesh_set(GraphicStuff &gs, int index){
	Mesh &mesh = gs.mesh_list[index];

	#ifndef __EMSCRIPTEN__
	glBindVertexArray(mesh.vao);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh.verticies.size() * sizeof(float),
		&mesh.verticies[0], GL_STATIC_DRAW);

	// POS
	glVertexAttribPointer(0, 2, GL_FLOAT,GL_FALSE, VERT_SZ*sizeof(float),
		(void *)0);
	glEnableVertexAttribArray(0);
	// UV
	glVertexAttribPointer(1, 2, GL_FLOAT,GL_FALSE, VERT_SZ*sizeof(float),
		(void *)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// COLOR
	glVertexAttribPointer(2, 4, GL_FLOAT,GL_FALSE, VERT_SZ*sizeof(float),
		(void *)(4 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// FLIP_COLOR
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, VERT_SZ*sizeof(float),
		(void *)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

	mesh.num_of_verticies = (int)mesh.verticies.size() / VERT_SZ;
	
	#else
	mesh.num_of_verticies = (int)mesh.verticies.size() / VERT_SZ;

	#endif
}

void mesh_draw(const GraphicStuff &graphic_stuff, int index) {
	const Mesh &mesh = graphic_stuff.mesh_list[index];

	if (mesh.num_of_verticies == 0) {
		return;
	}

	#ifndef __EMSCRIPTEN__
	glBindVertexArray(mesh.vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh.num_of_verticies);
	
	#else
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh.verticies.size() * sizeof(float),
		&mesh.verticies[0], GL_STATIC_DRAW);

	// POS
	glVertexAttribPointer(0, 2, GL_FLOAT,GL_FALSE, VERT_SZ*sizeof(float),
		(void *)0);
	glEnableVertexAttribArray(0);
	// UV
	glVertexAttribPointer(1, 2, GL_FLOAT,GL_FALSE, VERT_SZ*sizeof(float),
		(void *)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// COLOR
	glVertexAttribPointer(2, 4, GL_FLOAT,GL_FALSE, VERT_SZ*sizeof(float),
		(void *)(4 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// FLIP_COLOR
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, VERT_SZ*sizeof(float),
		(void *)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glDrawArrays(GL_TRIANGLES, 0, mesh.num_of_verticies);

	#endif
}

void mesh_release(GraphicStuff &gs, int index) {
	Mesh &mesh = gs.mesh_list[index];

	#ifndef __EMSCRIPTEN__
		glDeleteVertexArrays(1, &mesh.vao);
		glDeleteBuffers(1, &mesh.vbo);
	#else
		glDeleteBuffers(1, &mesh.vbo);
	#endif

	mesh.verticies.clear();
	mesh.num_of_verticies = 0;
	mesh.vao = 0;
	mesh.vbo = 0;

	mesh.running = false;
}
