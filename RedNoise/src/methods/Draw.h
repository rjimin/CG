#pragma once

#include <DrawingWindow.h>
#include <CanvasPoint.h>
#include <Colour.h>
#include <TextureMap.h>
#include <ModelTriangle.h>
#include <unordered_map>

class Draw {
public:
    static void drawGreyscale(DrawingWindow &window);
    static void draw2DColour(DrawingWindow &window);
    static void drawLine(CanvasPoint from, CanvasPoint to, Colour colour, DrawingWindow &window);
    static void drawStrokedTriangle(CanvasTriangle triangle, Colour colour, DrawingWindow &window);
    static void drawPoint(DrawingWindow &window, const CanvasPoint &point, Colour colour);
    static void drawWireframe(DrawingWindow &window, const glm::vec3 &cameraPosition, const glm::mat3 &cameraOrientation, float focalLength,
                              const std::vector<ModelTriangle> &triangles, std::vector<std::vector<float>> &depthBuffer);
    static void drawRasterisedScene(DrawingWindow &window, const glm::vec3 &cameraPosition, const glm::mat3 &cameraOrientation, float focalLength,
                                const std::vector<ModelTriangle> &triangles, std::vector<std::vector<float>> &depthBuffer);
    static void drawRayTracedScene(DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float focalLength,
                                   const std::vector<ModelTriangle> &triangles, const glm::vec3 &lightSource,
                                   std::unordered_map<int, glm::vec3> &vertexNormalMap, std::unordered_map<int, std::string> materialMap);
};
