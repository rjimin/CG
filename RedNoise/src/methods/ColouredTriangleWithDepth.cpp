#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <CanvasPoint.h>
#include <Colour.h>
#include "Triangle.h"
#include "Constants.h"
#include "ColouredTriangleWithDepth.h"

void drawDepthLine(CanvasPoint from, CanvasPoint to, Colour colour, DrawingWindow &window, std::vector<std::vector<float>> &depthBuffer) {
    uint32_t pixelColour = (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);

    float xDiff = to.x - from.x;
    float yDiff = to.y - from.y;
    float zDiff = to.depth - from.depth;

    float steps = fmax(abs(xDiff), abs(yDiff));
    float xStep = xDiff / steps;
    float yStep = yDiff / steps;
    float zStep = zDiff / steps;

    float x = from.x;
    float y = from.y;
    float depth = from.depth;

    for (float i = 0.0; i <= steps; i++) {
        int pixelX = static_cast<int>(x);
        int pixelY = static_cast<int>(y);

        if (pixelX >= 0 && pixelX < WIDTH && pixelY >= 0 && pixelY < HEIGHT) {
            if (depth > depthBuffer[pixelX][pixelY]) {
                depthBuffer[pixelX][pixelY] = depth;
                window.setPixelColour(pixelX, pixelY, pixelColour);
            }
        }
        x += xStep;
        y += yStep;
        depth += zStep;
    }
}

void fillFlatBottomTriangle(CanvasPoint topVertex, CanvasPoint bottomEdgeVertex1, CanvasPoint bottomEdgeVertex2, Colour colour, DrawingWindow &window,
                            std::vector<std::vector<float>> &depthBuffer) {
    float stepX1 = (bottomEdgeVertex1.x - topVertex.x) / (bottomEdgeVertex1.y - topVertex.y);
    float stepX2 = (bottomEdgeVertex2.x - topVertex.x) / (bottomEdgeVertex2.y - topVertex.y);

    float stepDepth1 = (bottomEdgeVertex1.depth - topVertex.depth) / (bottomEdgeVertex1.y - topVertex.y);
    float stepDepth2 = (bottomEdgeVertex2.depth - topVertex.depth) / (bottomEdgeVertex2.y - topVertex.y);

    float currentX1 = topVertex.x;
    float currentX2 = topVertex.x;

    float currentDepth1 = topVertex.depth;
    float currentDepth2 = topVertex.depth;

    for (float y = topVertex.y; y <= bottomEdgeVertex1.y; y++) {
        CanvasPoint from = {std::round(currentX1), y, currentDepth1};
        CanvasPoint to = {std::round(currentX2), y, currentDepth2};

        drawDepthLine(from, to, colour, window, depthBuffer);

        currentX1 += stepX1;
        currentX2 += stepX2;
        currentDepth1 += stepDepth1;
        currentDepth2 += stepDepth2;
    }
}

void fillFlatTopTriangle(CanvasPoint bottomVertex, CanvasPoint topEdgeVertex1, CanvasPoint topEdgeVertex2, Colour colour, DrawingWindow &window,
                         std::vector<std::vector<float>> &depthBuffer) {
    float stepX1 = (bottomVertex.x - topEdgeVertex1.x) / (bottomVertex.y - topEdgeVertex1.y);
    float stepX2 = (bottomVertex.x - topEdgeVertex2.x) / (bottomVertex.y - topEdgeVertex2.y);

    float stepDepth1 = (bottomVertex.depth - topEdgeVertex1.depth) / (bottomVertex.y - topEdgeVertex1.y);
    float stepDepth2 = (bottomVertex.depth - topEdgeVertex2.depth) / (bottomVertex.y - topEdgeVertex2.y);

    float currentX1 = topEdgeVertex1.x;
    float currentX2 = topEdgeVertex2.x;

    float currentDepth1 = topEdgeVertex1.depth;
    float currentDepth2 = topEdgeVertex2.depth;

    for (float y = topEdgeVertex1.y; y <= bottomVertex.y; y++) {
        CanvasPoint from = {std::round(currentX1), y, currentDepth1};
        CanvasPoint to = {std::round(currentX2), y, currentDepth2};

        drawDepthLine(from, to, colour, window, depthBuffer);

        currentX1 += stepX1;
        currentX2 += stepX2;
        currentDepth1 += stepDepth1;
        currentDepth2 += stepDepth2;
    }
}

void ColouredTriangleWithDepth::fillColouredTriangle(CanvasTriangle triangle, Colour colour, DrawingWindow &window, std::vector<std::vector<float>> &depthBuffer) {
    CanvasTriangle sortedTriangle = Triangle::sortVertices(triangle);

    CanvasPoint v0 = sortedTriangle.v0();
    CanvasPoint v1 = sortedTriangle.v1();
    CanvasPoint v2 = sortedTriangle.v2();

    if (v1.y == v2.y) {
        fillFlatBottomTriangle(v0, v1, v2, colour, window, depthBuffer);
    }
    else if (v0.y == v1.y) {
        fillFlatTopTriangle(v2, v0, v1, colour, window, depthBuffer);
    }
    else {
        float splitRatio = (v1.y - v0.y) / (v2.y - v0.y);
        float v3_X = v0.x + splitRatio * (v2.x - v0.x);
        float v3_Depth = v0.depth + splitRatio * (v2.depth - v0.depth);
        CanvasPoint v3 = {std::round(v3_X), v1.y, v3_Depth};

        fillFlatBottomTriangle(v0, v1, v3, colour, window, depthBuffer);
        fillFlatTopTriangle(v2, v1, v3, colour, window, depthBuffer);
    }
}
