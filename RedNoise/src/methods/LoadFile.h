#pragma once

#include <glm/glm.hpp>
#include <ModelTriangle.h>
#include <unordered_map>

class LoadFile {
public:
    static std::vector<ModelTriangle> triangles;
    static Colour colour;
    static std::unordered_map<int, glm::vec3> vertexNormalMap;
    static int getVertexIndex(const glm::vec3& vertex);
    static void loadObj();
};