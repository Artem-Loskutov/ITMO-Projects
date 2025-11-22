#include <fstream>
#include <sstream>
#include <vector>
#include <array>

#include "parcer.h"

Mesh::Mesh (std::string filename) {
    std::ifstream file(filename);
    std::string line;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line[0] == 'v' && line[1] == ' ') {
                std::stringstream ss(line.substr(2));
                vec3 v;
                ss >> v.x >> v.y >> v.z;
                vertices.push_back(v);
            }
            else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ') {
                std::stringstream ss(line.substr(3));
                vec2 vt;
                ss >> vt.x >> vt.y;
                texcoords.push_back(vt);
            }
            else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ') {
                std::stringstream ss(line.substr(3));
                vec3 vn;
                ss >> vn.x >> vn.y >> vn.z;
                normals.push_back(vn);
            }
            else if (line[0] == 'f' && line[1] == ' ') {
                for (char& letter : line) {
                    if (letter == '/') letter = ' ';
                }
                std::stringstream ss(line.substr(2));
                int v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3;
                ss >> v1 >> vt1 >> vn1 >> v2 >> vt2 >> vn2 >> v3 >> vt3 >> vn3;
                faces.push_back({ v1 - 1, v2 - 1, v3 - 1 });
                text_faces.push_back({ vt1 - 1, vt2 - 1, vt3 - 1 });
                norm_faces.push_back({ vn1 - 1, vn2 - 1, vn3 - 1 });
            }
        }
        file.close();
    }
}
