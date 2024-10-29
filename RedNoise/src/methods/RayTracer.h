#pragma once

#include "RayTriangleIntersection.h"

class RayTracer {
public:
    static bool intersectionFound;
    static RayTriangleIntersection getClosestIntersection(glm::vec3 cameraPosition, glm::vec3 rayDirection, const std::vector<ModelTriangle> &triangles);
};
