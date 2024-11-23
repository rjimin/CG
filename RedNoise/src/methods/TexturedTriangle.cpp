#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <TextureMap.h>
#include "Triangle.h"
#include "TexturedTriangle.h"

uint32_t TexturedTriangle::getTextureColour(TexturePoint texturePoint, TextureMap &textureMap) {
    int textureX = texturePoint.x * (textureMap.width - 1);
    int textureY = texturePoint.y * (textureMap.height - 1);

    int index = textureY * textureMap.width + textureX;

    return textureMap.pixels[index];
}

std::vector<CanvasPoint> interpolateCanvasPoints(CanvasPoint from, CanvasPoint to, float numberOfValues) {
    std::vector<CanvasPoint> result;

    float stepX = (to.x - from.x) / (numberOfValues - 1);
    float stepY = (to.y - from.y) / (numberOfValues - 1);

    for (int i = 0; i < numberOfValues; i++) {
        CanvasPoint canvasPoints;
        canvasPoints.x = from.x + stepX * i;
        canvasPoints.y = from.y + stepY * i;

        result.push_back(canvasPoints);
    }
    return result;
}

glm::vec3 calculateBarycentricCoords(CanvasPoint p, CanvasTriangle triangle) {
    CanvasPoint a = triangle.vertices[0];
    CanvasPoint b = triangle.vertices[1];
    CanvasPoint c = triangle.vertices[2];

    float v0x = b.x - a.x;
    float v0y = b.y - a.y;
    float v1x = c.x - a.x;
    float v1y = c.y - a.y;
    float v2x = p.x - a.x;
    float v2y = p.y - a.y;

    float d00 = v0x * v0x + v0y * v0y;
    float d01 = v0x * v1x + v0y * v1y;
    float d11 = v1x * v1x + v1y * v1y;
    float d20 = v2x * v0x + v2y * v0y;
    float d21 = v2x * v1x + v2y * v1y;

    float denom = d00 * d11 - d01 * d01;

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return {u, v, w};
}

TexturePoint calculateTexturePos(CanvasTriangle triangle, CanvasPoint p) {
    glm::vec3 barycentricCoords = calculateBarycentricCoords(p, triangle);

    float textureX = barycentricCoords.x * triangle.vertices[0].texturePoint.x +
                     barycentricCoords.y * triangle.vertices[1].texturePoint.x +
                     barycentricCoords.z * triangle.vertices[2].texturePoint.x;

    float textureY = barycentricCoords.x * triangle.vertices[0].texturePoint.y +
                     barycentricCoords.y * triangle.vertices[1].texturePoint.y +
                     barycentricCoords.z * triangle.vertices[2].texturePoint.y;

    return {textureX, textureY};
}

void drawTexturedLine(CanvasPoint from, CanvasPoint to, CanvasTriangle triangle, TextureMap &textureMap, DrawingWindow &window) {
    float xDiff = to.x - from.x;
    float yDiff = to.y - from.y;

    float numberOfSteps = fmax(abs(xDiff), abs(yDiff));
    std::vector<CanvasPoint> canvasPoints = interpolateCanvasPoints(from, to, numberOfSteps);

    for (size_t i = 0; i < canvasPoints.size(); i++) {
        CanvasPoint canvasPoint = canvasPoints[i];

        TexturePoint texturePoint = calculateTexturePos(triangle, canvasPoint);

        uint32_t colour = TexturedTriangle::getTextureColour(texturePoint, textureMap);
        window.setPixelColour(canvasPoint.x, canvasPoint.y, colour);
    }
}

void fillFlatBottomTriangle(CanvasPoint topVertex, CanvasPoint bottomEdge1, CanvasPoint bottomEdge2, TextureMap &textureMap, DrawingWindow &window) {
    float stepX1 = (bottomEdge1.x - topVertex.x) / (bottomEdge1.y - topVertex.y);
    float stepX2 = (bottomEdge2.x - topVertex.x) / (bottomEdge2.y - topVertex.y);

    float currentX1 = topVertex.x;
    float currentX2 = topVertex.x;

    CanvasTriangle triangle = {topVertex, bottomEdge1, bottomEdge2};

    for (float y = topVertex.y; y <= bottomEdge1.y; y++) {
        CanvasPoint from = {round(currentX1), y};
        CanvasPoint to = {round(currentX2), y};

        drawTexturedLine(from, to, triangle, textureMap, window);

        currentX1 += stepX1;
        currentX2 += stepX2;
    }
}

void fillFlatTopTriangle(CanvasPoint bottomVertex, CanvasPoint topEdge1, CanvasPoint topEdge2, TextureMap &textureMap, DrawingWindow &window) {
    float stepX1 = (bottomVertex.x - topEdge1.x) / (bottomVertex.y - topEdge1.y);
    float stepX2 = (bottomVertex.x - topEdge2.x) / (bottomVertex.y - topEdge2.y);

    float currentX1 = topEdge1.x;
    float currentX2 = topEdge2.x;

    CanvasTriangle triangle = {bottomVertex, topEdge1, topEdge2};

    for (float y = topEdge1.y; y <= bottomVertex.y; y++) {
        CanvasPoint from = {round(currentX1), y};
        CanvasPoint to = {round(currentX2), y};

        drawTexturedLine(from, to, triangle, textureMap, window);

        currentX1 += stepX1;
        currentX2 += stepX2;
    }
}

void TexturedTriangle::fillTexturedTriangle(CanvasTriangle triangle, TextureMap &textureMap, DrawingWindow &window) {
    CanvasTriangle sortedTriangle = Triangle::sortVertices(triangle);

    CanvasPoint v0 = sortedTriangle.v0();
    CanvasPoint v1 = sortedTriangle.v1();
    CanvasPoint v2 = sortedTriangle.v2();

    if (v1.y == v2.y) {
        fillFlatBottomTriangle(v0, v1, v2, textureMap, window);
    }
    else if (v0.y == v1.y) {
        fillFlatTopTriangle(v2, v0, v1, textureMap, window);
    }
    else {
        float splitRatio = (v1.y - v0.y) / (v2.y - v0.y);
        CanvasPoint v3 = {v0.x + splitRatio * (v2.x - v0.x), v1.y};
        v3.texturePoint = calculateTexturePos({v0, v1, v2}, v3);

        fillFlatBottomTriangle(v0, v1, v3, textureMap, window);
        fillFlatTopTriangle(v2, v1, v3, textureMap, window);
    }
}
