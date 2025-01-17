#pragma once

#include <glm/glm.hpp>
#include <ModelTriangle.h>
#include <unordered_map>
#include "TextureMap.h"

class LoadFile {
public:
    static std::vector<ModelTriangle> triangles;
    static std::unordered_map<int, glm::vec3> vertexNormalMap;
    static std::unordered_map<int, std::string> materialMap;
    static std::unordered_map<std::string, TextureMap> textureMaps;
    static int getVertexIndex(const glm::vec3 &vertex);
    static int getTriangleIndex(const ModelTriangle &triangle);
    static void loadObj();
};