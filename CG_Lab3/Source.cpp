#include "tgaimage.h"
#include "parcer.h"

#include <random>
#include <algorithm>
#include <array>
#include <iostream>

// Const vars
const int width = 800;
const int height = 800;
Vector3f light_dir = { 0, 0, 1 };

// Help structure
struct Vector2i {
	int x = 0;
	int y = 0;
	};
struct Vector3i {
	int x = 0;
	int y = 0;
	int z = 0;
};

// Move object to center of screen. [-1;1] => [0;2] with screen multiply
Vector3i screenCoord(Vector3f& meshVector) {
	return {
		(int)((meshVector.x + 1.0) * width / 2.0),
		(int)((meshVector.y + 1.0) * height / 2.0),
		(int)(meshVector.z * 100)
	};
}

// Find normal by two vertices
Vector3f findNormalVector(Vector3f& v0, Vector3f& v1) {
	return {
		v0.y * v1.z - v0.z * v1.y,
		v0.z * v1.x - v0.x * v1.z,
		v0.x * v1.y - v0.y * v1.x
	};
}

Vector3f barycentric(Vector2f A, Vector2f B, Vector2f C, Vector2f P) {
	Vector3f s0 = { C.x - A.x, B.x - A.x, A.x - P.x };
	Vector3f s1 = { C.y - A.y, B.y - A.y, A.y - P.y };

	Vector3f u = {
		s0.y * s1.z - s0.z * s1.y,
		s0.z * s1.x - s0.x * s1.z,
		s0.x * s1.y - s0.y * s1.x
	};

	if (std::abs(u.z) < 1e-2) return { -1, 1, 1 };

	return {
		1.0f - (u.x + u.y) / u.z,
		u.y / u.z,
		u.x / u.z
	};
}

// Comment me
void triangle(Vector3f& p0, Vector3f& p1, Vector3f& p2, TGAImage& image, TGAImage& texture, TGAColor color, int* zbuffer, Vector2f& uv0, Vector2f& uv1, Vector2f& uv2) {
	Vector3i screenP0 = screenCoord(p0);
	Vector3i screenP1 = screenCoord(p1);
	Vector3i screenP2 = screenCoord(p2);

	int minX = std::max(0, std::min({ screenP0.x, screenP1.x, screenP2.x }));
	int maxX = std::min(width - 1, std::max({ screenP0.x, screenP1.x, screenP2.x }));
	int minY = std::max(0, std::min({ screenP0.y, screenP1.y, screenP2.y }));
	int maxY = std::min(height - 1, std::max({ screenP0.y, screenP1.y, screenP2.y }));

	for (int x = minX; x <= maxX; x++) {
		for (int y = minY; y <= maxY; y++) {
			Vector3f bc = barycentric(
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

				Vector2f uv = {
					uv0.x * bc.x + uv1.x * bc.y + uv2.x * bc.z,
					uv0.y * bc.x + uv1.y * bc.y + uv2.y * bc.z
				};

				TGAColor texColor = texture.get(
					uv.x * texture.width(),
					(1 - uv.y) * texture.height()
				);

				image.set(x, y, texColor);
			}
		}
	}
	/*
	if (screenP0.y > screenP1.y) std::swap(screenP0, screenP1);
	if (screenP0.y > screenP2.y) std::swap(screenP0, screenP2);
	if (screenP1.y > screenP2.y) std::swap(screenP1, screenP2);

	// Anti not-triangle
	int area = 0.5 * abs((screenP1.x-screenP0.x)*(screenP2.y-screenP0.y)-(screenP2.x-screenP0.x)*(screenP1.y-screenP0.y));
	if (area == 0) return;

	int total_height = screenP2.y - screenP0.y;
	for (int i = 0; i < total_height; i++) {
		bool second_half = i > screenP1.y - screenP0.y || screenP1.y == screenP0.y;
		int segment_height = second_half ? screenP2.y - screenP1.y : screenP1.y - screenP0.y;
		float alpha = (float)i / total_height;
		float beta = (float)(i - (second_half ? screenP1.y - screenP0.y : 0)) / segment_height;
		Vector3i A = screenP0;
		A.x += (screenP2.x - screenP0.x) * alpha;
		A.y += (screenP2.y - screenP0.y) * alpha;
		Vector3i B;
		if (second_half) {
			B = screenP1;
			B.x += (screenP2.x - screenP1.x) * beta;
			B.y += (screenP2.y - screenP1.y) * beta;
		}
		else {
			B = screenP0;
			B.x += (screenP1.x - screenP0.x) * beta;
			B.y += (screenP1.y - screenP0.y) * beta;
		}
		if (A.x > B.x) std::swap(A, B);
		for (int j = A.x; j <= B.x; j++) {
			float phi = B.x == A.x ? 1 : (float)(j - A.x) / (float)(B.x - A.x);
			Vector3i P = {
				A.x + (B.x - A.x) * phi,
				A.y + (B.y - A.y) * phi,
				A.z + (B.z - A.z) * phi
			};
			int idx = P.x + P.y * width;
			if (zbuffer[idx] < P.z) {
				zbuffer[idx] = P.z;
				image.set(P.x, P.y, color);
			}
		}
	}*/
}

int main() {
	TGAImage image(width, height, TGAImage::RGB);
	image.flip_vertically();

	Mesh mesh = read("african_head.obj");
	
	TGAImage texture;
	texture.read_tga_file("african_head_diffuse.tga");

	int* zbuffer = new int[width * height];

	for (int i = 0; i < mesh.faces.size(); i++) {
		auto& f = mesh.faces[i];      // индексы вершин (v)
		auto& ft = mesh.tex_faces[i];  // индексы текстур (vt)

		Vector3f p0 = mesh.vertices[f[0]];
		Vector3f p1 = mesh.vertices[f[1]];
		Vector3f p2 = mesh.vertices[f[2]];

		Vector2f uv0 = mesh.texcoords[ft[0]];
		Vector2f uv1 = mesh.texcoords[ft[1]];
		Vector2f uv2 = mesh.texcoords[ft[2]];

		triangle(p0, p1, p2, image, texture, TGAColor(), zbuffer, uv0, uv1, uv2);
	}

	/*
	while(!mesh.faces.empty()){
		std::array<int,3> triangleFace = mesh.faces.back();
		std::vector<int> indexOfPoints = { (int)triangleFace[0] - 1 , (int)triangleFace[1] - 1, (int)triangleFace[2] - 1};

		Vector3f v0 = mesh.vertices[indexOfPoints[0]];
		Vector3f v1 = mesh.vertices[indexOfPoints[1]];
		Vector3f v2 = mesh.vertices[indexOfPoints[2]];

		Vector3f vec1 = {
			v1.x - v0.x,
			v1.y - v0.y,
			v1.z - v0.z
		};
		Vector3f vec2 = {
			v2.x - v0.x,
			v2.y - v0.y,
			v2.z - v0.z
		};
		Vector3f normalVector = findNormalVector(vec1, vec2);
		
		// Normalizing
		float len = sqrt(normalVector.x * normalVector.x + normalVector.y * normalVector.y + normalVector.z * normalVector.z);
		normalVector.x /= len;
		normalVector.y /= len;
		normalVector.z /= len;

		float intensity = normalVector.x * light_dir.x + normalVector.y * light_dir.y + normalVector.z * light_dir.z;

		if (intensity >= 0) {
			TGAColor color = { intensity * 255, intensity * 255, intensity * 255 ,255};
			triangle(mesh.vertices[indexOfPoints[0]], mesh.vertices[indexOfPoints[1]], mesh.vertices[indexOfPoints[2]], image, color, zbuffer);
		}
		mesh.faces.pop_back();
	}*/

	image.write_tga_file("output.tga");
	
	return 0;
}