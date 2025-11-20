#include <fstream>
#include <sstream>
#include <vector>
#include <array>

#include "parcer.h"

Mesh read(std::string filename) {
    std::ifstream file(filename);
    std::string line;
    Mesh mesh;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line[0] == 'v' && line[1] == ' ') {
                std::stringstream ss(line.substr(2));
                Vector3f v;
                ss >> v.x >> v.y >> v.z;
                mesh.vertices.push_back(v);
            }
            else if (line[0] == 'v' && line[1] == 't' && line[1] == 't') {
                std::stringstream ss(line.substr(3));
                Vector2f vt;
                ss >> vt.x >> vt.y;
                mesh.texcoords.push_back(vt);
            }
            else if (line[0] == 'f' && line[1] == ' ') {
                for (char& letter : line) {
                    if (letter == '/') letter = ' ';
                }
                std::stringstream ss(line.substr(2));
                int v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3;
                ss >> v1 >> vt1 >> vn1 >> v2 >> vt2 >> vn2 >> v3 >> vt3 >> vn3;
                mesh.faces.push_back({ v1 - 1, v2 - 1, v3 - 1 });
                mesh.tex_faces.push_back({ vt1 - 1, vt2 - 1, vt3 - 1 });
            }
        }
        file.close();
    }
    return mesh;
}
