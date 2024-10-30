#pragma once

#include <DrawingWindow.h>
#include <CanvasPoint.h>
#include <Colour.h>
#include <TextureMap.h>
#include <ModelTriangle.h>

class Draw {
public:
    static void drawGreyscale(DrawingWindow &window);
    static void draw2DColour(DrawingWindow &window);
    static void drawLine(CanvasPoint from, CanvasPoint to, Colour colour, DrawingWindow &window);
    static void drawStrokedTriangle(CanvasTriangle triangle, Colour colour, DrawingWindow &window);
    static void drawTexturedLine(CanvasPoint from, CanvasPoint to, TexturePoint fromTP, TexturePoint toTP, TextureMap &textureMap, DrawingWindow &window);
    static void drawPoint(DrawingWindow &window, const CanvasPoint &point, Colour colour);
    static void drawWireframe(DrawingWindow &window, const glm::vec3 &cameraPosition, const glm::mat3 &cameraOrientation, float focalLength,
                              const std::vector<ModelTriangle> &triangles);
    static void drawDepthLine(CanvasPoint from, CanvasPoint to, Colour colour, DrawingWindow &window, std::vector<std::vector<float>> &depthBuffer);
    static void drawRasterisedScene(DrawingWindow &window, const glm::vec3 &cameraPosition, const glm::mat3 &cameraOrientation, float focalLength,
                                const std::vector<ModelTriangle> &triangles, std::vector<std::vector<float>> &depthBuffer);
    static void drawRayTracedScene(DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float focalLength,
                                   const std::vector<ModelTriangle> &triangles, std::vector<std::vector<float>> &depthBuffer, const glm::vec3 &lightSource);
};
