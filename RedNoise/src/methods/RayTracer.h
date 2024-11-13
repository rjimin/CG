#pragma once

#include <unordered_map>
#include "RayTriangleIntersection.h"

class RayTracer {
public:
    static bool intersectionFound;
    static RayTriangleIntersection getClosestIntersection(glm::vec3 cameraPosition, glm::vec3 rayDirection, const std::vector<ModelTriangle> &triangles,
                                                          size_t selfShadowExcludeIndex);
    static bool isShadowed(const glm::vec3 &surfacePoint, const glm::vec3 &lightSource, const std::vector<ModelTriangle> &triangles, size_t triangleIndex);
    static float calculateSinglePointBrightness(glm::vec3 &cameraPosition, const glm::vec3 &intersectionPoint, const glm::vec3 &normal,
                                                const glm::vec3 &lightSource);
    static float calculateClusterBrightness(glm::vec3 &cameraPosition, const glm::vec3 &intersectionPoint, const glm::vec3 &normal,
                                            const std::vector<glm::vec3> &lightCluster);
    static glm::vec3 calculateBarycentricCoords(glm::vec3 p, ModelTriangle triangle);
    static float getGouraudShading(glm::vec3 &cameraPosition, const glm::vec3 &lightSource, glm::vec3 intersectionPoint, const ModelTriangle& triangle,
                                   std::unordered_map<int, glm::vec3> &vertexNormalMap);
    static float getPhongShading(glm::vec3 &cameraPosition, const glm::vec3 &lightSource, glm::vec3 intersectionPoint, const ModelTriangle& triangle,
                                 std::unordered_map<int, glm::vec3> &vertexNormalMap);
    static std::vector<glm::vec3> generateLightCluster(const glm::vec3 &lightSource, int numLightPoints);
    static float calculateSoftShadow(const glm::vec3 &surfacePoint, const std::vector<glm::vec3> &lightCluster,
                                     const std::vector<ModelTriangle> &triangles, size_t triangleIndex);
};
