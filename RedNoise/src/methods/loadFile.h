#pragma once

#include <glm/glm.hpp>
#include <ModelTriangle.h>

class loadFile {
public:
    static std::vector<glm::vec3> vertices;
    static std::vector<ModelTriangle> triangles;
    static Colour colour;
    static void loadObj();
};