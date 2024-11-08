#include <vector>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include "Interpolation.h"

std::vector<float> Interpolation::interpolateSingleFloats(float from, float to, int numberOfValues) {
    std::vector<float> result;
    float step = (to - from) / (numberOfValues - 1);
    for (int i = 0; i < numberOfValues; i++) {
        result.push_back(from + step * i);
    }
    return result;
}

std::vector<glm::vec3> Interpolation::interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfValues) {
    std::vector<glm::vec3> result;

    float stepX = (to.x - from.x) / (numberOfValues - 1);
    float stepY = (to.y - from.y) / (numberOfValues - 1);
    float stepZ = (to.z - from.z) / (numberOfValues - 1);

    for (int i = 0; i < numberOfValues; i++) {
        glm::vec3 interpolatedVec3;
        interpolatedVec3.x = from.x + stepX * i;
        interpolatedVec3.y = from.y + stepY * i;
        interpolatedVec3.z = from.z + stepZ * i;

        result.push_back(interpolatedVec3);
    }
    return result;
}
