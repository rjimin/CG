#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <CanvasPoint.h>

class Interpolation {
public:
    static std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues);
    static std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfValues);
    static std::vector<CanvasPoint> interpolateCanvasPoints(CanvasPoint from, CanvasPoint to, float numberOfValues);
    static std::vector<TexturePoint> interpolateTexturePoints(TexturePoint from, TexturePoint to, float numberOfValues);
};