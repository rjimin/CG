#include <CanvasPoint.h>
#include <glm/glm.hpp>
#include "Constants.h"
#include "Projection.h"

CanvasPoint Projection::projectVertexOntoCanvasPoint(glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength, glm::vec3 vertexPosition) {
    glm::vec3 cameraToVertex = vertexPosition - cameraPosition;
    glm::vec3 cameraSpacePos = cameraToVertex * cameraOrientation;

    float x = cameraSpacePos.x;
    float y = cameraSpacePos.y;
    float z = cameraSpacePos.z;

    if (z >= 0) {
        std::cerr << "Skipping vertex with invalid z-value: " << z << std::endl;
        return {};
    }

    CanvasPoint imagePlanePos;

    float scalingFactor = 160.0f;

    imagePlanePos.x = -((x * focalLength) / z * scalingFactor) + (WIDTH / 2.0f);
    imagePlanePos.y = ((y * focalLength) / z * scalingFactor) + (HEIGHT / 2.0f);
    imagePlanePos.depth = 1.0f / std::abs(z);

    return imagePlanePos;
}