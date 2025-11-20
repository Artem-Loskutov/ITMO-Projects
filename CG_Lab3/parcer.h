#pragma once

struct Vector3f
{
	float x = 0;
	float y = 0;
	float z = 0;
};
struct Vector2f
{
	float x = 0;
	float y = 0;
};

struct Mesh {
	std::vector<Vector3f> vertices;
	std::vector<Vector2f> texcoords;
	std::vector<std::array<int, 3>> faces;
	std::vector<std::array<int, 3>> tex_faces;
};

Mesh read(std::string filename);