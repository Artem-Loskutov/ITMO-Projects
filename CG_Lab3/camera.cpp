#include "camera.h"
#include "geometry.h"

mat4 Camera::viewport(int x, int y, int w, int h, int d) {
    mat4 m = identify<4,4>();
    m[0][3] = x + w / 2.f;
    m[1][3] = y + h / 2.f;
    m[2][3] = d / 2.f;

    m[0][0] = w / 2.f;
    m[1][1] = h / 2.f;
    m[2][2] = d / 2.f;
    return m;
}

mat4 Camera::projection(vec3 eye, vec3 center) {
    mat4 Projection = identify<4,4>();
    Projection[3][2] = -1.f / norm(eye - center);
    return Projection;
}

mat4 Camera::lookAt(vec3 eye, vec3 center, vec3 up) {
    vec3 z = normalized(eye - center);
    vec3 x = normalized(cross(up, z));
    vec3 y = normalized(cross(z, x));
    mat4 res = identify<4,4>();
    for (int i = 0; i < 3; i++) {
        res[0][i] = x[i];
        res[1][i] = y[i];
        res[2][i] = z[i];
        res[i][3] = -center[i];
    }
    return res;
}
