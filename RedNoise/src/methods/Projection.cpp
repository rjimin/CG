#include <CanvasPoint.h>
#include <glm/glm.hpp>
#include "Constants.h"
#include "Projection.h"

CanvasPoint Projection::projectVertexOntoCanvasPoint(glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength, glm::vec3 vertexPosition) {
    glm::vec3 cameraToVertex = vertexPosition - cameraPosition;
    glm::vec3 adjustedVector = cameraToVertex * cameraOrientation;

    float x = adjustedVector.x;
    float y = adjustedVector.y;
    float z = adjustedVector.z;

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