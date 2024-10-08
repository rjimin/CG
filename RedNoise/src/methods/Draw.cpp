#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <vector>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <Colour.h>
#include <TextureMap.h>
#include "Interpolation.h"
#include "TexturedTriangle.h"
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
    float xDiff = to.x -from.x;
    float yDiff = to.y -from.y;

    float numberOfSteps = fmax(abs(xDiff), abs(yDiff));

    float xStepSize = xDiff / numberOfSteps;
    float yStepSize = yDiff / numberOfSteps;

    uint32_t lineColour = (255 << 24) + (uint32_t(colour.red) << 16) + (uint32_t(colour.green) << 8) + uint32_t(colour.blue);

    for (float i = 0.0; i < numberOfSteps; i++) {
        float x = from.x + (xStepSize * i);
        float y = from.y + (yStepSize * i);
        window.setPixelColour(x, y, lineColour);
    }
}

void Draw::drawStrokedTriangle(CanvasTriangle triangle, Colour colour, DrawingWindow &window) {
    drawLine(triangle.v0(), triangle.v1(), colour, window);
    drawLine(triangle.v1(), triangle.v2(), colour, window);
    drawLine(triangle.v2(), triangle.v0(), colour, window);
}

void Draw::drawTexturedLine(CanvasPoint from, CanvasPoint to, TexturePoint fromTP, TexturePoint toTP, TextureMap &textureMap, DrawingWindow &window) {
    float xDiff = to.x -from.x;
    float yDiff = to.y -from.y;

    float numberOfSteps = fmax(abs(xDiff), abs(yDiff));

    std::vector<CanvasPoint> canvasPoints = Interpolation::interpolateCanvasPoints(from, to, numberOfSteps);
    std::vector<TexturePoint> texturePoints = Interpolation::interpolateTexturePoints(fromTP, toTP, numberOfSteps);

    for (float i = 0.0; i < canvasPoints.size(); i++) {
        CanvasPoint canvasPoint = canvasPoints[i];
        TexturePoint texturePoint = texturePoints[i];

        uint32_t colour = TexturedTriangle::getTextureColour(texturePoint, textureMap);
        window.setPixelColour(canvasPoint.x, canvasPoint.y, colour);
    }
}