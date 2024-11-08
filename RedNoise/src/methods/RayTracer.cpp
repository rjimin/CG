#include <vector>
#include <RayTriangleIntersection.h>
#include "RayTracer.h"

bool RayTracer::intersectionFound = false;

RayTriangleIntersection RayTracer::getClosestIntersection(glm::vec3 cameraPosition, glm::vec3 rayDirection, const std::vector<ModelTriangle> &triangles,
                                                          size_t selfShadowExcludeIndex) {
    float smallestT = std::numeric_limits<float>::max();
    RayTriangleIntersection closestIntersection;
    intersectionFound = false;

    for (size_t triangleIndex = 0; triangleIndex < triangles.size(); triangleIndex++) {
        const ModelTriangle &triangle = triangles[triangleIndex];

        if (triangleIndex == selfShadowExcludeIndex) continue;

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
    }
    return closestIntersection;
}

bool RayTracer::isShadowed(const glm::vec3 &surfacePoint, const glm::vec3 &lightSource, const std::vector<ModelTriangle> &triangles, size_t triangleIndex) {
    glm::vec3 shadowRayDirection = glm::normalize(lightSource - surfacePoint);
    float lightDistance = glm::length(lightSource - surfacePoint);
    RayTriangleIntersection shadowIntersection = getClosestIntersection(surfacePoint, shadowRayDirection, triangles, triangleIndex);
    return intersectionFound && shadowIntersection.distanceFromCamera < lightDistance;
}

float RayTracer::calculateBrightness(glm::vec3 &cameraPosition, const glm::vec3 &intersectionPoint, const glm::vec3 &normal, const glm::vec3 &lightSource) {
    float brightness = 0.0f;
    glm::vec3 lightDirection = glm::normalize(lightSource - intersectionPoint);

    float distance = glm::length(lightSource - intersectionPoint);
    float proximityBrightness = 1.0f / (4.0f * M_PI * pow(distance, 2));

    float AOIBrightness = glm::dot(normal, lightDirection);

    float specularExponent = 256.0f;
    glm::vec3 viewDirection = glm::normalize(cameraPosition - intersectionPoint);
    glm::vec3 reflectionDirection = glm::normalize(lightDirection - 2.0f * normal * glm::dot(lightDirection, normal));
    float specularFactor = glm::dot(viewDirection, reflectionDirection);
    float specularBrightness = pow(glm::max(specularFactor, 0.0f), specularExponent);

    brightness = proximityBrightness + AOIBrightness + specularBrightness;

    float ambientThreshold = 0.1f;
    brightness = glm::max(brightness, ambientThreshold);

    return glm::clamp(brightness, 0.0f, 1.0f);
}

glm::vec3 RayTracer::calculateBarycentricCoords(const glm::vec3 &p, const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c) {
    glm::vec3 v0 = b - a;
    glm::vec3 v1 = c - a;
    glm::vec3 v2 = p - a;

    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);

    float denom = d00 * d11 - d01 * d01;

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return glm::vec3(u, v, w);
}
