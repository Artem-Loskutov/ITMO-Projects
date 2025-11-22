#pragma once

#include "geometry.h"

class Camera
{
private:
	vec3 eye;
	vec3 center;
	vec3 up;
public:
	Camera(vec3 eye_, vec3 center_, vec3 up_) : eye(eye_), center(center_), up(up_) {}
	mat4 viewport(int x, int y, int w, int h, int d);
	mat4 projection(vec3 eye, vec3 center);
	mat4 lookAt(vec3 eye, vec3 center, vec3 up);
};