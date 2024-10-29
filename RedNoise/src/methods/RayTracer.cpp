#include <vector>
#include <RayTriangleIntersection.h>
#include "RayTracer.h"

bool RayTracer::intersectionFound = false;

RayTriangleIntersection RayTracer::getClosestIntersection(glm::vec3 cameraPosition, glm::vec3 rayDirection, const std::vector<ModelTriangle> &triangles) {
    float smallestT = std::numeric_limits<float>::max();
    size_t triangleIndex = 0;
    RayTriangleIntersection closestIntersection;
    intersectionFound = false;

    for (const ModelTriangle &triangle: triangles) {

        glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
        glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
        glm::vec3 SPVector = cameraPosition - triangle.vertices[0];
        glm::mat3 DEMatrix(-rayDirection, e0, e1);
        glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

        float t = possibleSolution.x;
        float u = possibleSolution.y;
        float v = possibleSolution.z;

        if (t > 0.0
            && (u >= 0.0) && (u <= 1.0)
            && (v >= 0.0) && (v <= 1.0)
            && (u + v) <= 1.0) {
            intersectionFound = true;

            if (t < smallestT) {
                smallestT = t;

                glm::vec3 r = cameraPosition + t * rayDirection;

                closestIntersection = RayTriangleIntersection(
                        r,
                        t,
                        triangle,
                        triangleIndex
                );
            }
        }
        triangleIndex++;
    }
    return closestIntersection;
}
