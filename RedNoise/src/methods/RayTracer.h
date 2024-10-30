#pragma once

#include "RayTriangleIntersection.h"

class RayTracer {
public:
    static bool intersectionFound;
    static RayTriangleIntersection getClosestIntersection(glm::vec3 cameraPosition, glm::vec3 rayDirection, const std::vector<ModelTriangle> &triangles, size_t selfShadowExcludeIndex);
    static bool isShadowed(const glm::vec3 &surfacePoint, const glm::vec3 &lightSource, const std::vector<ModelTriangle> &triangles, size_t triangleIndex);
};
