#pragma once

#include <glm/glm.hpp>

class Projection {
public:
    static CanvasPoint projectVertexOntoCanvasPoint(glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength, glm::vec3 vertexPosition);
};