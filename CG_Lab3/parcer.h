#pragma once
#include "geometry.h"

struct Mesh {
	std::vector<vec3> vertices;
	std::vector<vec2> texcoords;
	std::vector<vec3> normals;
	std::vector<std::vector<int>> faces;
	std::vector<std::vector<int>> text_faces;
	std::vector<std::vector<int>> norm_faces;

	Mesh (std::string filename);
	int totalFaces() { return faces.size(); }
};
