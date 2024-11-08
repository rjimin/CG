#include <CanvasTriangle.h>
#include <vector>
#include <CanvasPoint.h>
#include <ModelTriangle.h>
#include "glm/vec3.hpp"
#include "Constants.h"
#include "Triangle.h"

CanvasTriangle Triangle::RandomTriangle() {
    CanvasPoint v0 = {static_cast<float>(rand() % WIDTH), static_cast<float>(rand() % HEIGHT)};
    CanvasPoint v1 = {static_cast<float>(rand() % WIDTH), static_cast<float>(rand() % HEIGHT)};
    CanvasPoint v2 = {static_cast<float>(rand() % WIDTH), static_cast<float>(rand() % HEIGHT)};

    return CanvasTriangle{v0, v1, v2};
}

CanvasTriangle Triangle::sortVertices(CanvasTriangle triangle) {
    CanvasPoint v0 = triangle.v0();
    CanvasPoint v1 = triangle.v1();
    CanvasPoint v2 = triangle.v2();

    // v0: top vertex(has the smallest y-coord),
    // v1: middle vertex,
    // v2: bottom vertex(has the largest y-coord)
    if (v0.y > v1.y) std::swap(v0, v1);
    if (v0.y > v2.y) std::swap(v0, v2);
    if (v1.y > v2.y) std::swap(v1, v2);

    return CanvasTriangle(v0, v1, v2);
}

glm::vec3 Triangle::calculateCenter(const std::vector<ModelTriangle> &triangles) {
    glm::vec3 minCoords = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxCoords = glm::vec3(std::numeric_limits<float>::lowest());

    for (const ModelTriangle &triangle : triangles) {
        for (const glm::vec3 &vertex : triangle.vertices) {
            minCoords.x = std::min(minCoords.x, vertex.x);
            minCoords.y = std::min(minCoords.y, vertex.y);
            minCoords.z = std::min(minCoords.z, vertex.z);

            maxCoords.x = std::max(maxCoords.x, vertex.x);
            maxCoords.y = std::max(maxCoords.y, vertex.y);
            maxCoords.z = std::max(maxCoords.z, vertex.z);
        }
    }
    return (minCoords + maxCoords) / 2.0f;
}