#pragma once

#include <glm/glm.hpp>
#include <ModelTriangle.h>

class LoadFile {
public:
    static std::vector<ModelTriangle> triangles;
    static Colour colour;
    static void loadObj();
};