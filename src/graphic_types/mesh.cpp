#include "mesh.h"

#include "graphic_types.h"

#include <vector>

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

int mesh_new(GraphicStuff &graphic_stuff,const std::vector<float> &verticies){
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
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(float),
		&verticies[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT,GL_FALSE, 2*sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	mesh.num_of_verticies = (int)verticies.size() / 2;

	#else
	glGenBuffers(1, &mesh.vbo);
	mesh.verticies = verticies;
	
	#endif

	return index;
}

void draw_mesh(const GraphicStuff &graphic_stuff, int index) {
	const Mesh &mesh = graphic_stuff.mesh_list[index];

	#ifndef __EMSCRIPTEN__
	glBindVertexArray(mesh.vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh.num_of_verticies);
	
	#else
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh.verticies.size() * sizeof(float),
		&mesh.verticies[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT,GL_FALSE, 2*sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, (int)mesh.verticies.size() / 2);

	#endif
}
