#pragma once

#include <glm/glm.hpp>

class Projection {
public:
    static CanvasPoint projectVertexOntoCanvasPoint(glm::vec3 cameraPosition, float focalLength, glm::vec3 vertexPosition);
};