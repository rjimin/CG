#include <CanvasPoint.h>
#include <glm/glm.hpp>
#include "Constants.h"
#include "Projection.h"

CanvasPoint Projection::projectVertexOntoCanvasPoint(glm::vec3 cameraPosition, float focalLength, glm::vec3 vertexPosition) {
    glm::vec3 cameraCoords = vertexPosition - cameraPosition;

    float x = cameraCoords.x;
    float y = cameraCoords.y;
    float z = cameraCoords.z;

    if (z >= 0) {
        std::cerr << "Skipping vertex with invalid z-value: " << z << std::endl;
        return {};
    }

    CanvasPoint projectedCanvasPoints;

    float scalingFactor = 160.0f;

    projectedCanvasPoints.x = -((x * focalLength) / z * scalingFactor) + (WIDTH / 2.0f);
    projectedCanvasPoints.y = ((y * focalLength) / z * scalingFactor) + (HEIGHT / 2.0f);

    return projectedCanvasPoints;
}