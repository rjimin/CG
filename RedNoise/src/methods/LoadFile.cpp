#include <Utils.h>
#include <Colour.h>
#include <TextureMap.h>
#include <ModelTriangle.h>
#include <unordered_map>
#include <glm/glm.hpp>
#include <sstream>
#include "LoadFile.h"

std::vector<ModelTriangle> LoadFile::triangles;
std::unordered_map<int, glm::vec3> LoadFile::vertexNormalMap;
std::unordered_map<int, std::string> LoadFile::materialMap;
Colour colour;
std::vector<glm::vec3> vertices;

int LoadFile::getVertexIndex(const glm::vec3 &vertex) {
    for (size_t i = 0; i < vertices.size(); ++i) {
        if (vertices[i] == vertex) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int LoadFile::getTriangleIndex(const ModelTriangle &triangle) {
    for (size_t i = 0; i < triangles.size(); ++i) {
        if (triangles[i].vertices[0] == triangle.vertices[0]
         && triangles[i].vertices[1] == triangle.vertices[1]
         && triangles[i].vertices[2] == triangle.vertices[2]) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

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
    std::ifstream objFile("models/cornell-box-sphere-metal.obj");

    if (!objFile.is_open()) {
        std::cerr << "Error: Could not open OBJ file" << std::endl;
        return;
    }

    std::string line;
    std::unordered_map<int, std::vector<int>> vertexFaceMap;

    int faceIndex = 0;

    std::string material;

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

            int findIndexV1 = getVertexIndex(vertices[indexV1 - 1]);
            int findIndexV2 = getVertexIndex(vertices[indexV2 - 1]);
            int findIndexV3 = getVertexIndex(vertices[indexV3 - 1]);

            if (findIndexV1 == -1) {
                vertexFaceMap[indexV1 - 1].push_back(faceIndex);
            } else {
                vertexFaceMap[findIndexV1].push_back(faceIndex);
            }
            if (findIndexV2 == -1) {
                vertexFaceMap[indexV2 - 1].push_back(faceIndex);
            } else {
                vertexFaceMap[findIndexV2].push_back(faceIndex);
            }
            if (findIndexV3 == -1) {
                vertexFaceMap[indexV3 - 1].push_back(faceIndex);
            } else {
                vertexFaceMap[findIndexV3].push_back(faceIndex);
            }

            triangles.push_back(triangle);
            materialMap[faceIndex] = material;
            faceIndex++;
        }
        else if (identifier == "usemtl") {
            lineStream >> material;
            if (colourMap.find(material) != colourMap.end()) {
                glm::vec3 rgb = colourMap[material];
                colour = Colour(rgb.r * 255, rgb.g * 255, rgb.b * 255);
            }
        }
    }
    objFile.close();

    for (const auto& entry : vertexFaceMap) {
        int vertexIndex = entry.first;
        const std::vector<int>& faceIndices = entry.second;

        glm::vec3 vertexNormal(0.0f, 0.0f, 0.0f);
        for (int face : faceIndices) {
            vertexNormal += triangles[face].normal;
        }
        vertexNormal = glm::normalize(vertexNormal / static_cast<float>(faceIndices.size()));
        vertexNormalMap[vertexIndex] = vertexNormal;
    }
}
