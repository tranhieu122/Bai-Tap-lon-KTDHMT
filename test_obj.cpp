#include <iostream>
#include <sstream>
#include <vector>
#include <string>

struct Vector3 { float x, y, z; };
struct MeshFace { int v[3], n[3], t[3]; };
struct Mesh {
    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<MeshFace> faces;
};

const char* HUMAN_MODEL_DATA = 
"v 0.00 1.63 0.08\nv -0.11 1.61 0.16\nv 0.11 1.61 0.16\nv 0.11 1.61 -0.01\nv -0.11 1.61 -0.01\n"
"vn 0 1 0\nvn 0 0 1\n"
"f 1//1 2//1 3//1\nf 1//1 3//1 4//1\n";

bool loadOBJFromStream(std::istream& in, Mesh& mesh) {
    mesh.vertices.clear();
    mesh.normals.clear();
    mesh.faces.clear();
    
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        if (line[0] == 'v' && line[1] == ' ') {
            std::istringstream s(line.substr(2));
            Vector3 v; s >> v.x >> v.y >> v.z;
            mesh.vertices.push_back(v);
        } else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ') {
            std::istringstream s(line.substr(3));
            Vector3 n; s >> n.x >> n.y >> n.z;
            mesh.normals.push_back(n);
        } else if (line[0] == 'f' && line[1] == ' ') {
            std::istringstream s(line.substr(2));
            MeshFace f;
            std::string part;
            for (int i = 0; i < 3; i++) {
                if (!(s >> part)) break;
                size_t firstSlash = part.find('/');
                size_t lastSlash = part.find_last_of('/');
                
                f.v[i] = std::stoi(part.substr(0, firstSlash)) - 1;
                f.n[i] = -1; f.t[i] = -1;
                
                if (firstSlash != std::string::npos && lastSlash != std::string::npos) {
                    if (lastSlash > firstSlash + 1) {
                        f.t[i] = std::stoi(part.substr(firstSlash + 1, lastSlash - firstSlash - 1)) - 1;
                    }
                    f.n[i] = std::stoi(part.substr(lastSlash + 1)) - 1;
                }
            }
            mesh.faces.push_back(f);
        }
    }
    return true;
}

int main() {
    Mesh m;
    std::istringstream iss(HUMAN_MODEL_DATA);
    loadOBJFromStream(iss, m);
    std::cout << "Vertices: " << m.vertices.size() << std::endl;
    std::cout << "Normals: " << m.normals.size() << std::endl;
    std::cout << "Faces: " << m.faces.size() << std::endl;
    if(m.faces.size() > 0) {
        std::cout << "Face 0: " << m.faces[0].v[0] << "," << m.faces[0].n[0] << std::endl;
    }
    return 0;
}
