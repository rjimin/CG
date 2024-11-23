#include <Utils.h>
#include <Colour.h>
#include <TextureMap.h>
#include <ModelTriangle.h>
#include <unordered_map>
#include <glm/glm.hpp>
#include <sstream>
#include "Constants.h"
#include "LoadFile.h"

std::vector<ModelTriangle> LoadFile::triangles;
std::unordered_map<int, glm::vec3> LoadFile::vertexNormalMap;
std::unordered_map<int, std::string> LoadFile::materialMap;
std::unordered_map<std::string, TextureMap> LoadFile::textureMaps;

Colour colour;
std::vector<glm::vec3> vertices;
std::vector<glm::vec2> texturePoints;

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
    std::ifstream mtlFile(baseDirectory + "cornell-box.mtl");

    if (!mtlFile.is_open()) {
        std::cerr << "Error: Could not open MTL file" << std::endl;
        return {};
    }

    std::string line;
    std::unordered_map<std::string, glm::vec3> colourMap;
    std::string material;

    while (getline(mtlFile, line)) {
        if (line.empty()) continue;

        std::istringstream lineStream(line);
        std::string identifier;
        lineStream >> identifier;

        if (identifier == "newmtl") {
            lineStream >> material;
        }
        else if (identifier == "Kd") {
            float r, g, b;
            lineStream >> r >> g >> b;

            glm::vec3 rgb(r, g, b);

            if (!material.empty()) {
                colourMap[material] = rgb;
            }
        }
        else if (identifier == "map_Kd") {
            std::string textureFilename;
            lineStream >> textureFilename;

            if (!textureFilename.empty() && !material.empty()) {
                LoadFile::textureMaps[material] = TextureMap(baseDirectory + textureFilename);
            }
        }
    }
    mtlFile.close();

    return colourMap;
}

void LoadFile::loadObj() {
    std::unordered_map<std::string, glm::vec3> colourMap = loadMaterials();
    std::ifstream objFile(baseDirectory + "cornell-box.obj");

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
        else if (identifier == "vt") {
            float u, v;
            lineStream >> u >> v;
            texturePoints.emplace_back(u, v);
        }
        else if (identifier == "f") {
            std::vector<int> vertexIndices, textureIndices;
            std::string faceElement;

            while (lineStream >> faceElement) {
                std::istringstream faceStream(faceElement);
                std::string vertexIndex, textureIndex;

                std::getline(faceStream, vertexIndex, '/');
                vertexIndices.push_back(std::stoi(vertexIndex) - 1);

                if (std::getline(faceStream, textureIndex, '/')) {
                    if (!textureIndex.empty()) {
                        textureIndices.push_back(std::stoi(textureIndex) - 1);
                    }
                }
            }

            if (vertexIndices.size() == 3) {
                ModelTriangle triangle(
                        vertices[vertexIndices[0]],
                        vertices[vertexIndices[1]],
                        vertices[vertexIndices[2]],
                        colour
                );

                if (textureIndices.size() == 3) {
                    triangle.texturePoints[0] = TexturePoint(
                            texturePoints[textureIndices[0]].x,
                            texturePoints[textureIndices[0]].y
                    );
                    triangle.texturePoints[1] = TexturePoint(
                            texturePoints[textureIndices[1]].x,
                            texturePoints[textureIndices[1]].y
                    );
                    triangle.texturePoints[2] = TexturePoint(
                            texturePoints[textureIndices[2]].x,
                            texturePoints[textureIndices[2]].y
                    );
                }

                glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
                glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
                triangle.normal = glm::normalize(glm::cross(e0, e1));

                for (int i = 0; i < 3; ++i) {
                    int vertexIndex = vertexIndices[i];
                    int foundIndex = getVertexIndex(vertices[vertexIndex]);
                    if (foundIndex == -1) {
                        vertexFaceMap[vertexIndex].push_back(faceIndex);
                    } else {
                        vertexFaceMap[foundIndex].push_back(faceIndex);
                    }
                }

                triangles.push_back(triangle);
                materialMap[faceIndex] = material;
                faceIndex++;
            }
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
