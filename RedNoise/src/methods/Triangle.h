#pragma once

#include <CanvasTriangle.h>
#include <ModelTriangle.h>
#include "glm/vec3.hpp"

class Triangle {
public:
    static CanvasTriangle RandomTriangle();
    static CanvasTriangle sortVertices(CanvasTriangle triangle);
    static glm::vec3 calculateCenter(const std::vector<ModelTriangle> &triangles);
};