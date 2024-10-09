#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <CanvasPoint.h>
#include <Colour.h>
#include "Draw.h"
#include "Triangle.h"
#include "ColouredTriangleWithDepth.h"

void fillTopColouredTriangle(CanvasPoint topVertex, CanvasPoint bottomEdge1, CanvasPoint bottomEdge2, Colour colour, DrawingWindow &window, std::vector<std::vector<float>> &depthBuffer) {
    float stepX1 = (bottomEdge1.x - topVertex.x) / (bottomEdge1.y - topVertex.y);
    float stepX2 = (bottomEdge2.x - topVertex.x) / (bottomEdge2.y - topVertex.y);

    float stepDepth1 = (bottomEdge1.depth - topVertex.depth) / (bottomEdge1.y - topVertex.y);
    float stepDepth2 = (bottomEdge2.depth - topVertex.depth) / (bottomEdge2.y - topVertex.y);

    float currentX1 = topVertex.x;
    float currentX2 = topVertex.x;

    float currentDepth1 = topVertex.depth;
    float currentDepth2 = topVertex.depth;

    for (float y = topVertex.y; y <= bottomEdge1.y; y++) {
        CanvasPoint from = {currentX1, y, currentDepth1};
        CanvasPoint to = {currentX2, y, currentDepth2};

        Draw::drawDepthLine(from, to, colour, window, depthBuffer);

        currentX1 += stepX1;
        currentX2 += stepX2;

        currentDepth1 += stepDepth1;
        currentDepth2 += stepDepth2;
    }
}

void fillBottomColouredTriangle(CanvasPoint bottomVertex, CanvasPoint topEdge1, CanvasPoint topEdge2, Colour colour, DrawingWindow &window, std::vector<std::vector<float>> &depthBuffer) {
    float stepX1 = (bottomVertex.x - topEdge1.x) / (bottomVertex.y - topEdge1.y);
    float stepX2 = (bottomVertex.x - topEdge2.x) / (bottomVertex.y - topEdge2.y);

    float stepDepth1 = (bottomVertex.depth - topEdge1.depth) / (bottomVertex.y - topEdge1.y);
    float stepDepth2 = (bottomVertex.depth - topEdge2.depth) / (bottomVertex.y - topEdge2.y);

    float currentX1 = topEdge1.x;
    float currentX2 = topEdge2.x;

    float currentDepth1 = topEdge1.depth;
    float currentDepth2 = topEdge2.depth;

    for (float y = topEdge1.y; y <= bottomVertex.y; y++) {
        CanvasPoint from = {currentX1, y, currentDepth1};
        CanvasPoint to = {currentX2, y, currentDepth2};

        Draw::drawDepthLine(from, to, colour, window, depthBuffer);

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
        fillTopColouredTriangle(v0, v1, v2, colour, window, depthBuffer);
    }
    else if (v0.y == v1.y) {
        fillBottomColouredTriangle(v2, v0, v1, colour, window, depthBuffer);
    }
    else {
        float splitRatio = (v1.y - v0.y) / (v2.y - v0.y);
        float v3_X = v0.x + splitRatio * (v2.x - v0.x);
        float v3_Depth = v0.depth + splitRatio * (v2.depth - v0.depth);
        CanvasPoint v3 = {v3_X, v1.y, v3_Depth};

        fillTopColouredTriangle(v0, v1, v3, colour, window, depthBuffer);
        fillBottomColouredTriangle(v2, v1, v3, colour, window, depthBuffer);
    }
}
