#include <Utils.h>
#include <Colour.h>
#include <TextureMap.h>
#include <ModelTriangle.h>
#include <unordered_map>
#include <glm/glm.hpp>
#include <sstream>
#include "LoadFile.h"

std::vector<ModelTriangle> LoadFile::triangles;
Colour LoadFile::colour;

std::unordered_map<std::string, glm::vec3> loadMaterials() {
    std::ifstream mtlFile("models/cornell-box.mtl");

    if (!mtlFile.is_open()) {
        std::cerr << "Error: Could not open MTL file" << std::endl;
        return {};
    }

    std::string line;
    std::unordered_map<std::string, glm::vec3> colourMap;
    std::string colourName;

    while (getline(mtlFile, line)) {
        if (line.empty()) continue;

        std::istringstream lineStream(line);
        std::string identifier;
        lineStream >> identifier;

        if (identifier == "Kd") {
            float r, g, b;
            lineStream >> r >> g >> b;

            glm::vec3 rgb(r, g, b);

            if (!colourName.empty()) {
                colourMap[colourName] = rgb;
                colourName.clear();
            }
        }
        else if (identifier == "newmtl") {
            lineStream >> colourName;
        }
    }
    mtlFile.close();

    return colourMap;
}

void LoadFile::loadObj() {
    std::unordered_map<std::string, glm::vec3> colourMap = loadMaterials();

    std::ifstream objFile("models/cornell-box.obj");

    if (!objFile.is_open()) {
        std::cerr << "Error: Could not open OBJ file" << std::endl;
        return;
    }

    std::string line;
    std::vector<glm::vec3> vertices;

    while (getline(objFile, line)) {
        if(line.empty()) continue;

        std::istringstream lineStream(line);
        std::string identifier;
        lineStream >> identifier;

        if (identifier == "v") {
            float x, y, z;
            float scalingFactor = 0.35f;

            lineStream >> x >> y >> z;

            glm::vec3 vertex(x * scalingFactor, y * scalingFactor, z * scalingFactor);
            vertices.push_back(vertex);
        }
        else if (identifier == "f") {
            int indexV1, indexV2, indexV3;
            char slash;
            lineStream >> indexV1 >> slash >> indexV2 >> slash >> indexV3;

            ModelTriangle triangle(vertices[indexV1 - 1], vertices[indexV2 - 1], vertices[indexV3 - 1], colour);

            glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
            glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
            triangle.normal = glm::normalize(glm::cross(e0, e1));

            triangles.push_back(triangle);
        }
        else if (identifier == "usemtl") {
            std::string colourName;
            lineStream >> colourName;

            if (colourMap.find(colourName) != colourMap.end()) {
                glm::vec3 rgb = colourMap[colourName];
                colour = Colour(rgb.r * 255, rgb.g * 255, rgb.b * 255);
            }
        }
    }
    objFile.close();
}
