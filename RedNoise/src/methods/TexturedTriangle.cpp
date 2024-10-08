#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <TextureMap.h>
#include "Draw.h"
#include "Triangle.h"
#include "TexturedTriangle.h"

uint32_t TexturedTriangle::getTextureColour(TexturePoint texturePoint, TextureMap &textureMap) {
    int texX = texturePoint.x * (textureMap.width - 1);
    int texY = texturePoint.y * (textureMap.height - 1);

    int index = texY * textureMap.width + texX;

    return textureMap.pixels[index];
}

void TexturedTriangle::fillTopTexturedTriangle(CanvasPoint topVertex, CanvasPoint bottomEdge1, CanvasPoint bottomEdge2, TextureMap &textureMap, DrawingWindow &window) {
    float stepX1 = (bottomEdge1.x - topVertex.x) / (bottomEdge1.y - topVertex.y);
    float stepX2 = (bottomEdge2.x - topVertex.x) / (bottomEdge2.y - topVertex.y);

    float currentX1 = topVertex.x;
    float currentX2 = topVertex.x;

    for (float y = topVertex.y; y <= bottomEdge1.y; y++) {
        CanvasPoint from = {round(currentX1), y};
        CanvasPoint to = {round(currentX2), y};

        float disRatio = (y - topVertex.y) / (bottomEdge1.y - topVertex.y);
        TexturePoint fromTP = {topVertex.texturePoint.x + disRatio * (bottomEdge1.texturePoint.x - topVertex.texturePoint.x),
                               topVertex.texturePoint.y + disRatio * (bottomEdge1.texturePoint.y - topVertex.texturePoint.y)};
        TexturePoint toTP = {topVertex.texturePoint.x + disRatio * (bottomEdge2.texturePoint.x - topVertex.texturePoint.x),
                             topVertex.texturePoint.y + disRatio * (bottomEdge2.texturePoint.y - topVertex.texturePoint.y)};

        Draw::drawTexturedLine(from, to, fromTP, toTP, textureMap, window);

        currentX1 += stepX1;
        currentX2 += stepX2;
    }
}

void TexturedTriangle::fillBottomTexturedTriangle(CanvasPoint bottomVertex, CanvasPoint topEdge1, CanvasPoint topEdge2, TextureMap &textureMap, DrawingWindow &window) {
    float stepX1 = (bottomVertex.x - topEdge1.x) / (bottomVertex.y - topEdge1.y);
    float stepX2 = (bottomVertex.x - topEdge2.x) / (bottomVertex.y - topEdge2.y);

    float currentX1 = topEdge1.x;
    float currentX2 = topEdge2.x;

    for (float y = topEdge1.y; y <= bottomVertex.y; y++) {
        CanvasPoint from = {round(currentX1), y};
        CanvasPoint to = {round(currentX2), y};

        float disRatio = (y - topEdge1.y) / (bottomVertex.y - topEdge1.y);
        TexturePoint fromTP = {topEdge1.texturePoint.x + disRatio * (bottomVertex.texturePoint.x - topEdge1.texturePoint.x),
                               topEdge1.texturePoint.y + disRatio * (bottomVertex.texturePoint.y - topEdge1.texturePoint.y)};
        TexturePoint toTP = {topEdge2.texturePoint.x + disRatio * (bottomVertex.texturePoint.x - topEdge2.texturePoint.x),
                             topEdge2.texturePoint.y + disRatio * (bottomVertex.texturePoint.y - topEdge2.texturePoint.y)};

        Draw::drawTexturedLine(from, to, fromTP, toTP, textureMap, window);

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
        fillTopTexturedTriangle(v0, v1, v2, textureMap, window);
    }
    else if (v0.y == v1.y) {
        fillBottomTexturedTriangle(v2, v0, v1, textureMap, window);
    }
    else {
        float splitRatio = (v1.y - v0.y) / (v2.y - v0.y);
        CanvasPoint v3 = {v0.x + splitRatio * (v2.x - v0.x), v1.y};
        v3.texturePoint.x = v0.texturePoint.x + splitRatio * (v2.texturePoint.x - v0.texturePoint.x);
        v3.texturePoint.y = v0.texturePoint.y + splitRatio * (v2.texturePoint.y - v0.texturePoint.y);

        fillTopTexturedTriangle(v0, v1, v3, textureMap, window);
        fillBottomTexturedTriangle(v2, v1, v3, textureMap, window);
    }
}