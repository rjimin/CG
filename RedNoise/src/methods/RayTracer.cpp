#include <vector>
#include <RayTriangleIntersection.h>
#include <unordered_map>
#include "LoadFile.h"
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
    glm::vec3 offsetPoint = surfacePoint + shadowRayDirection * 1e-4f;

    RayTriangleIntersection shadowIntersection = getClosestIntersection(offsetPoint, shadowRayDirection, triangles, triangleIndex);

    return intersectionFound && shadowIntersection.distanceFromCamera < lightDistance;
}

float calculateDiffuseLighting(glm::vec3 lightDirection, const glm::vec3 &intersectionPoint, const glm::vec3 &normal, const glm::vec3 &lightSource) {
    float distance = glm::length(lightSource - intersectionPoint);
    float proximityLighting = 1.0f / (4.0f * M_PI * pow(distance, 2));
    proximityLighting = glm::clamp(proximityLighting, 0.0f, 1.0f);

    float AOILighting = glm::dot(normal, lightDirection);
    AOILighting = glm::clamp(AOILighting, 0.0f, 1.0f);

    float diffuseLighting = proximityLighting + AOILighting;
    diffuseLighting = glm::clamp(diffuseLighting, 0.0f, 1.0f);

    return diffuseLighting;
}

float calculateSpecularHighlighting(glm::vec3 lightDirection, glm::vec3 &cameraPosition, const glm::vec3 &intersectionPoint, const glm::vec3 &normal, const glm::vec3 &lightSource) {
    float specularExponent = 256.0f;
    glm::vec3 viewDirection = glm::normalize(cameraPosition - intersectionPoint);
    glm::vec3 reflectionDirection = glm::normalize(lightDirection - 2.0f * normal * glm::dot(lightDirection, normal));
    float specularFactor = glm::dot(viewDirection, reflectionDirection);
    float specularHighlighting = pow(glm::max(specularFactor, 0.0f), specularExponent);
    specularHighlighting = glm::clamp(specularHighlighting, 0.0f, 1.0f);

    return specularHighlighting;
}

float RayTracer::calculateSinglePointBrightness(glm::vec3 &cameraPosition, const glm::vec3 &intersectionPoint, const glm::vec3 &normal,
                                                const glm::vec3 &lightSource) {
    glm::vec3 lightDirection = glm::normalize(lightSource - intersectionPoint);

    float diffuseLighting = calculateDiffuseLighting(lightDirection, intersectionPoint, normal, lightSource);

    float specularHighlighting = calculateSpecularHighlighting(lightDirection, cameraPosition, intersectionPoint, normal, lightSource);

    float brightness = diffuseLighting + specularHighlighting;

    float ambientThreshold = 0.1f;
    brightness = glm::max(brightness, ambientThreshold);

    return glm::clamp(brightness, 0.0f, 1.0f);
}

float RayTracer::calculateClusterBrightness(glm::vec3 &cameraPosition, const glm::vec3 &intersectionPoint, const glm::vec3 &normal,
                                            const std::vector<glm::vec3> &lightCluster) {
    float totalBrightness = 0.0f;

    for (const glm::vec3 &light : lightCluster) {
        float brightness = calculateSinglePointBrightness(cameraPosition, intersectionPoint, normal, light);
        totalBrightness += brightness;
    }
    totalBrightness /= lightCluster.size();

    return totalBrightness;
}

glm::vec3 RayTracer::calculateBarycentricCoords(glm::vec3 p, ModelTriangle triangle) {
    glm::vec3 a = triangle.vertices[0];
    glm::vec3 b = triangle.vertices[1];
    glm::vec3 c = triangle.vertices[2];

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

    return {u, v, w};
}

std::vector<glm::vec3> getVertexNormal(const ModelTriangle& triangle, std::unordered_map<int, glm::vec3> &vertexNormalMap) {
    int indexV0 = LoadFile::getVertexIndex(triangle.vertices[0]);
    int indexV1 = LoadFile::getVertexIndex(triangle.vertices[1]);
    int indexV2 = LoadFile::getVertexIndex(triangle.vertices[2]);

    glm::vec3 normalV0 = vertexNormalMap[indexV0];
    glm::vec3 normalV1 = vertexNormalMap[indexV1];
    glm::vec3 normalV2 = vertexNormalMap[indexV2];

    return {normalV0, normalV1, normalV2};
}

float RayTracer::getGouraudShading(glm::vec3 &cameraPosition, const glm::vec3 &lightSource, glm::vec3 intersectionPoint, const ModelTriangle& triangle,
                                   std::unordered_map<int, glm::vec3> &vertexNormalMap) {
    std::vector<glm::vec3> normals = getVertexNormal(triangle, vertexNormalMap);
    glm::vec3 normalV0 = normals[0];
    glm::vec3 normalV1 = normals[1];
    glm::vec3 normalV2 = normals[2];

    glm::vec3 barycentricCoords = RayTracer::calculateBarycentricCoords(intersectionPoint, triangle);

    float brightnessV0 = RayTracer::calculateSinglePointBrightness(cameraPosition, triangle.vertices[0], normalV0, lightSource);
    float brightnessV1 = RayTracer::calculateSinglePointBrightness(cameraPosition, triangle.vertices[1], normalV1, lightSource);
    float brightnessV2 = RayTracer::calculateSinglePointBrightness(cameraPosition, triangle.vertices[2], normalV2, lightSource);

    float interpolatedBrightness = barycentricCoords.x * brightnessV0 +
                                   barycentricCoords.y * brightnessV1 +
                                   barycentricCoords.z * brightnessV2;

    return interpolatedBrightness;
}

float RayTracer::getPhongShading(glm::vec3 &cameraPosition, const glm::vec3 &lightSource, glm::vec3 intersectionPoint, const ModelTriangle& triangle,
                                 std::unordered_map<int, glm::vec3> &vertexNormalMap) {
    std::vector<glm::vec3> normals = getVertexNormal(triangle, vertexNormalMap);
    glm::vec3 normalV0 = normals[0];
    glm::vec3 normalV1 = normals[1];
    glm::vec3 normalV2 = normals[2];

    glm::vec3 barycentricCoords = RayTracer::calculateBarycentricCoords(intersectionPoint, triangle);

    glm::vec3 interpolatedNormal = glm::normalize(barycentricCoords.x * normalV0
                                                  + barycentricCoords.y * normalV1
                                                  + barycentricCoords.z * normalV2);

    float brightness = calculateSinglePointBrightness(cameraPosition, intersectionPoint, interpolatedNormal, lightSource);

    return brightness;
}

std::vector<glm::vec3> RayTracer::generateLightCluster(const glm::vec3 &lightSource, int numLightPoints) {
    std::vector<glm::vec3> lightCluster;

    lightCluster.push_back(lightSource);

    int layers = std::sqrt(numLightPoints);
    int remainingPoints = numLightPoints;
    float radiusStep = 0.1f;
    float zStep = 0.05f;

    for (int layer = 1; layer <= layers; layer++) {
        int pointsPerLayer = std::max(1, remainingPoints / (layers - layer + 1));
        remainingPoints -= pointsPerLayer;

        float radius = layer * radiusStep;
        float angleStep = 2.0f * M_PI / pointsPerLayer;
        float zOffset = (layer - layers / 2.0f) * zStep;

        for (int i = 0; i < pointsPerLayer; ++i) {
            float angle = i * angleStep;

            glm::vec3 point;
            point.x = lightSource.x + radius * std::cos(angle);
            point.y = lightSource.y + radius * std::sin(angle);
            point.z = lightSource.z + zOffset;

            lightCluster.push_back(point);
        }
    }
    return lightCluster;
}

float RayTracer::calculateSoftShadow(const glm::vec3 &surfacePoint, const std::vector<glm::vec3> &lightCluster,
                                     const std::vector<ModelTriangle> &triangles, size_t triangleIndex) {
    int totalShadows = 0;
    for (const glm::vec3 &light : lightCluster) {
        if (isShadowed(surfacePoint, light, triangles, triangleIndex)) {
            totalShadows++;
        }
    }
    return static_cast<float>(totalShadows) / lightCluster.size();
}
