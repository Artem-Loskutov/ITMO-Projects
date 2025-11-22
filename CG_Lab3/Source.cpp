#include "tgaimage.h"
#include "parcer.h"

#include <random>
#include <algorithm>
#include <array>
#include <iostream>

// Const vars
const int width = 800;
const int height = 800;
vec3 light_dir = { 0, 0, 1 };

// Move object to center of screen. [-1;1] => [0;2] with screen multiply
vec3 screenCoord(vec3& meshVector) {
	return {
		(meshVector.x + 1.0) * width / 2.0,
		(meshVector.y + 1.0) * height / 2.0,
		meshVector.z * 100
	};
}

vec3 barycentric(vec2 A, vec2 B, vec2 C, vec2 P) {
	vec3 s0 = { C.x - A.x, B.x - A.x, A.x - P.x };
	vec3 s1 = { C.y - A.y, B.y - A.y, A.y - P.y };

	vec3 u = cross(s0, s1);

	if (std::abs(u.z) < 1e-2) return { -1, 1, 1 };

	return {
		1.0f - (u.x + u.y) / u.z,
		u.y / u.z,
		u.x / u.z
	};
}

// Comment me
void triangle(vec3& p0, vec3& p1, vec3& p2, TGAImage& image, TGAImage& texture, int* zbuffer, vec2& uv0, vec2& uv1, vec2& uv2) {
	vec3 screenP0 = screenCoord(p0);
	vec3 screenP1 = screenCoord(p1);
	vec3 screenP2 = screenCoord(p2);

	int minX = std::max(0, std::min({ (int)screenP0.x, (int)screenP1.x, (int)screenP2.x }));
	int maxX = std::min(width - 1, std::max({ (int)screenP0.x, (int)screenP1.x, (int)screenP2.x }));
	int minY = std::max(0, std::min({ (int)screenP0.y, (int)screenP1.y, (int)screenP2.y }));
	int maxY = std::min(height - 1, std::max({ (int)screenP0.y, (int)screenP1.y, (int)screenP2.y }));



	TGAColor color = { rand() % 255, rand() % 255, rand() % 255, 255 };

	for (int x = minX; x <= maxX; x++) {
		for (int y = minY; y <= maxY; y++) {
			vec3 bc = barycentric(
				{ (float)screenP0.x, (float)screenP0.y},
				{ (float)screenP1.x, (float)screenP1.y},
				{ (float)screenP2.x, (float)screenP2.y},
				{ (float)x, (float)y}
			);

			if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;
			float z = screenP0.z * bc.x + screenP1.z * bc.y + screenP2.z * bc.z;
			int idx = x + y * width;

			if (zbuffer[idx] < z) {
				zbuffer[idx] = z;
				/*
				vec2 uv = {
					uv0.x * bc.x + uv1.x * bc.y + uv2.x * bc.z,
					uv0.y * bc.x + uv1.y * bc.y + uv2.y * bc.z
				};

				TGAColor texColor = texture.get(
					uv.x * texture.width(),
					(1 - uv.y) * texture.height()
				);

				image.set(x, y, texColor);*/

				image.set(x, y, color);
			}
		}
	}
}

int main() {
	TGAImage image(width, height, TGAImage::RGB);
	image.flip_vertically();

	Mesh mesh("sponza.obj");
	
	TGAImage texture;
	texture.read_tga_file("african_head_diffuse.tga");

	int* zbuffer = new int[width * height];

	for (int i = 0; i < mesh.faces.size(); i++) {
		auto& f = mesh.faces[i];
		auto& ft = mesh.text_faces[i];

		vec3 p0 = mesh.vertices[f[0]];
		vec3 p1 = mesh.vertices[f[1]];
		vec3 p2 = mesh.vertices[f[2]];

		vec2 uv0 = mesh.texcoords[ft[0]];
		vec2 uv1 = mesh.texcoords[ft[1]];
		vec2 uv2 = mesh.texcoords[ft[2]];

		triangle(p0, p1, p2, image, texture, zbuffer, uv0, uv1, uv2);
	}

	image.write_tga_file("output.tga");
	
	return 0;
}/*
#include "tgaimage.h"
#include "parcer.h"
#include "camera.h"
#include <iostream>
#include <algorithm>

const int width = 800;
const int height = 800;
const int depth = 255;

// barycentric, cross, proj<2> предполагаются из geometry.h

vec3 barycentric(vec2 A, vec2 B, vec2 C, vec2 P) {
	vec3 s0 = { C.x - A.x, B.x - A.x, A.x - P.x };
	vec3 s1 = { C.y - A.y, B.y - A.y, A.y - P.y };
	vec3 u = cross(s0, s1);
	if (std::abs(u.z) < 1e-2) return { -1,1,1 };
	return { 1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z };
}

// Triangle с трансформацией через MVP
void triangle (vec4* pts, TGAImage& image, TGAImage& zbuffer, int* zbuf) {
	vec2 bboxmin(1e8, 1e8);
	vec2 bboxmax(-1e8, -1e8);
	for (int i = 0; i < 3; i++) for (int j = 0; j < 2; j++) {
		bboxmin[j] = std::min(bboxmin[j], pts[i][j] / pts[i][3]);
		bboxmax[j] = std::max(bboxmax[j], pts[i][j] / pts[i][3]);
	}

	TGAColor color(rand() % 256, rand() % 256, rand() % 256, 255);

	for (int x = (int)bboxmin.x; x <= (int)bboxmax.x; x++) {
		for (int y = (int)bboxmin.y; y <= (int)bboxmax.y; y++) {
			vec2 P(x, y);
			vec3 c = barycentric(
				proj<4, 2>(pts[0] / pts[0][3]),
				proj<4, 2>(pts[1] / pts[1][3]),
				proj<4, 2>(pts[2] / pts[2][3]),
				P
			);
			if (x < 0 || x >= width || y < 0 || y >= height) continue;
			int idx = x + y * width;
			float z = pts[0][2] * c.x + pts[1][2] * c.y + pts[2][2] * c.z;
			if (zbuf[idx] < z) {
				zbuf[idx] = z;
				image.set(x, y, color);
			}
		}
	}
}

int main() {
	TGAImage image(width, height, TGAImage::RGB);
	int* zbuf = new int[width * height];
	std::fill(zbuf, zbuf + width * height, -100000); // минимальные значения

	Mesh mesh("sponza.obj");


	// Камера
	vec3 eye(1, 2, 0);

	vec3 center(0, 0, 0);
	vec3 up(0, 1, 0);
	Camera cam(eye, center, up);
	mat4 model = identify<4,4>();
	mat4 view = cam.lookAt(eye, center, up);
	mat4 proj = cam.projection(eye, center);
	mat4 viewport = cam.viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4, depth);
	mat4 MVP = viewport * proj * view * model;

	// Проходим по треугольникам
	int j = 0;
	for (auto& f : mesh.faces) {
		vec4 screen_coords[3];
		for (int i = 0; i < 3; i++) {
			vec3 v = mesh.vertices[f[i]];
			screen_coords[i] = embed<4>(v);
			screen_coords[i] = MVP * screen_coords[i]; // трансформация через камеру
		}
		triangle(screen_coords, image, image, zbuf);
		if (j % 100 == 0)std::cout << j << std::endl;
		j++;
	}

	image.flip_vertically();
	image.write_tga_file("output.tga");
	delete[] zbuf;
	return 0;
}*/