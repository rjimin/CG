#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <vector>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <Colour.h>
#include <ModelTriangle.h>
#include "Constants.h"
#include "Interpolation.h"
#include "Projection.h"
#include "ColouredTriangleWithDepth.h"
#include "RayTriangleIntersection.h"
#include "RayTracer.h"
#include "LoadFile.h"
#include "Draw.h"

void Draw::drawGreyscale(DrawingWindow &window) {
    window.clearPixels();
    std::vector<float> greyValues = Interpolation::interpolateSingleFloats(255, 0, window.width);
    for (size_t y = 0; y < window.height; y++) {
        for (size_t x = 0; x < window.width; x++) {
            auto greyValue = greyValues[x];
            uint32_t colour = (255 << 24) + (int(greyValue) << 16) + (int(greyValue) << 8) + int(greyValue);
            window.setPixelColour(x, y, colour);
        }
    }
}

void Draw::draw2DColour(DrawingWindow &window) {
    window.clearPixels();
    glm::vec3 topLeft(255, 0, 0);        // red
    glm::vec3 topRight(0, 0, 255);       // blue
    glm::vec3 bottomRight(0, 255, 0);    // green
    glm::vec3 bottomLeft(255, 255, 0);   // yellow

    std::vector<glm::vec3> firstCol = Interpolation::interpolateThreeElementValues(topLeft, bottomLeft, window.height);
    std::vector<glm::vec3> lastCol = Interpolation::interpolateThreeElementValues(topRight, bottomRight, window.height);

    for (size_t y = 0; y < window.height; y++) {
        for (size_t x = 0; x < window.width; x++) {
            std::vector<glm::vec3> eachRow = Interpolation::interpolateThreeElementValues(firstCol[y], lastCol[y], window.width);
            glm::vec3 pixel = eachRow[x];
            uint32_t colour = (255 << 24) + (uint32_t(pixel[0]) << 16) + (uint32_t(pixel[1]) << 8) + uint32_t(pixel[2]);
            window.setPixelColour(x, y, colour);
        }
    }
}

void Draw::drawLine(CanvasPoint from, CanvasPoint to, Colour colour, DrawingWindow &window) {
    float xDiff = to.x - from.x;
    float yDiff = to.y - from.y;

    float numberOfSteps = fmax(abs(xDiff), abs(yDiff));

    float xStep = xDiff / numberOfSteps;
    float yStep = yDiff / numberOfSteps;

    uint32_t lineColour = (255 << 24) + (uint32_t(colour.red) << 16) + (uint32_t(colour.green) << 8) + uint32_t(colour.blue);

    for (float i = 0.0; i <= numberOfSteps; i++) {
        int x = static_cast<int>(from.x + (xStep * i));
        int y = static_cast<int>(from.y + (yStep * i));

        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            window.setPixelColour(x, y, lineColour);
        }
    }
}

void Draw::drawStrokedTriangle(CanvasTriangle triangle, Colour colour, DrawingWindow &window) {
    drawLine(triangle.v0(), triangle.v1(), colour, window);
    drawLine(triangle.v1(), triangle.v2(), colour, window);
    drawLine(triangle.v2(), triangle.v0(), colour, window);
}

void Draw::drawPoint(DrawingWindow &window, const CanvasPoint &point, Colour colour) {
    uint32_t pointColour = (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);
    window.setPixelColour(point.x, point.y, pointColour);
}

void Draw::drawWireframe(DrawingWindow &window, const glm::vec3 &cameraPosition, const glm::mat3 &cameraOrientation, float focalLength,
                         const std::vector<ModelTriangle> &triangles, std::vector<std::vector<float>> &depthBuffer) {
    window.clearPixels();
    for (std::vector<float> &row: depthBuffer) std::fill(row.begin(), row.end(), 0.0f);

    for (const ModelTriangle &triangle : triangles) {

        CanvasPoint v0 = Projection::projectVertexOntoCanvasPoint(cameraPosition, cameraOrientation, focalLength, triangle.vertices[0]);
        CanvasPoint v1 = Projection::projectVertexOntoCanvasPoint(cameraPosition, cameraOrientation, focalLength, triangle.vertices[1]);
        CanvasPoint v2 = Projection::projectVertexOntoCanvasPoint(cameraPosition, cameraOrientation, focalLength, triangle.vertices[2]);

        CanvasTriangle canvasTriangle = {v0, v1, v2};

        Draw::drawStrokedTriangle(canvasTriangle, Colour{255, 255, 255}, window);
    }
}

void Draw::drawRasterisedScene(DrawingWindow &window, const glm::vec3 &cameraPosition, const glm::mat3 &cameraOrientation, float focalLength,
                               const std::vector<ModelTriangle> &triangles, std::vector<std::vector<float>> &depthBuffer) {
    window.clearPixels();
    for (std::vector<float> &row: depthBuffer) std::fill(row.begin(), row.end(), 0.0f);

    for (const ModelTriangle &triangle : triangles) {
        CanvasPoint v0 = Projection::projectVertexOntoCanvasPoint(cameraPosition, cameraOrientation, focalLength, triangle.vertices[0]);
        CanvasPoint v1 = Projection::projectVertexOntoCanvasPoint(cameraPosition, cameraOrientation, focalLength, triangle.vertices[1]);
        CanvasPoint v2 = Projection::projectVertexOntoCanvasPoint(cameraPosition, cameraOrientation, focalLength, triangle.vertices[2]);

        if (v0.depth == 0 || v1.depth == 0 || v2.depth == 0) {
            std::cerr << "Skipping vertex with invalid depth" << std::endl;
            continue;
        }

        CanvasTriangle canvasTriangle = {v0, v1, v2};

        ColouredTriangleWithDepth::fillColouredTriangle(canvasTriangle, triangle.colour, window, depthBuffer);
    }
}

void Draw::drawRayTracedScene(DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float focalLength,
                              const std::vector<ModelTriangle> &triangles, const glm::vec3 &lightSource) {
    window.clearPixels();

    float scalingFactor = 160.0f;

    for (size_t y = 0; y < HEIGHT; y++) {
        for (size_t x = 0; x < WIDTH; x++) {

            glm::vec3 imagePlanePos((x - WIDTH / 2.0f) / scalingFactor, -(y - HEIGHT / 2.0f) / scalingFactor, -focalLength);
            glm::vec3 rayDirection = glm::normalize(cameraOrientation * imagePlanePos);

            RayTriangleIntersection intersection = RayTracer::getClosestIntersection(cameraPosition, rayDirection, triangles, -1);

            if (RayTracer::intersectionFound && intersection.distanceFromCamera > 0) {
                float shadow = RayTracer::calculateSoftShadow(intersection.intersectionPoint, lightSource, triangles, intersection.triangleIndex);

                float brightness = (1.0f - shadow) * RayTracer::calculateBrightness(cameraPosition, intersection.intersectionPoint, intersection.intersectedTriangle.normal, lightSource);

                uint8_t red = static_cast<uint8_t>(intersection.intersectedTriangle.colour.red * brightness);
                uint8_t green = static_cast<uint8_t>(intersection.intersectedTriangle.colour.green * brightness);
                uint8_t blue = static_cast<uint8_t>(intersection.intersectedTriangle.colour.blue * brightness);

                uint32_t colour = (255 << 24) + (red << 16) + (green << 8) + blue;
                window.setPixelColour(x, y, colour);
            }
        }
    }
}
