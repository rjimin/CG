#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <CanvasPoint.h>
#include <Colour.h>
#include "Draw.h"
#include "Triangle.h"
#include "ColouredTriangle.h"

void ColouredTriangle::fillTopColouredTriangle(CanvasPoint topVertex, CanvasPoint bottomEdge1, CanvasPoint bottomEdge2, Colour colour, DrawingWindow &window) {
    float stepX1 = (bottomEdge1.x - topVertex.x) / (bottomEdge1.y - topVertex.y);
    float stepX2 = (bottomEdge2.x - topVertex.x) / (bottomEdge2.y - topVertex.y);

    float currentX1 = topVertex.x;
    float currentX2 = topVertex.x;

    for (float y = topVertex.y; y <= bottomEdge1.y; ++y) {
        CanvasPoint from = {currentX1, y};
        CanvasPoint to = {currentX2, y};

        Draw::drawLine(from, to, colour, window);

        currentX1 += stepX1;
        currentX2 += stepX2;
    }
}

void ColouredTriangle::fillBottomColouredTriangle(CanvasPoint bottomVertex, CanvasPoint topEdge1, CanvasPoint topEdge2, Colour colour, DrawingWindow &window) {
    float stepX1 = (bottomVertex.x - topEdge1.x) / (bottomVertex.y - topEdge1.y);
    float stepX2 = (bottomVertex.x - topEdge2.x) / (bottomVertex.y - topEdge2.y);

    float currentX1 = topEdge1.x;
    float currentX2 = topEdge2.x;

    for (float y = topEdge1.y; y <= bottomVertex.y; ++y) {
        CanvasPoint from = {currentX1, y};
        CanvasPoint to = {currentX2, y};

        Draw::drawLine(from, to, colour, window);

        currentX1 += stepX1;
        currentX2 += stepX2;
    }
}

void ColouredTriangle::fillColouredTriangle(CanvasTriangle triangle, Colour colour, DrawingWindow &window) {
    CanvasTriangle sortedTriangle = Triangle::sortVertices(triangle);

    CanvasPoint v0 = sortedTriangle.v0();
    CanvasPoint v1 = sortedTriangle.v1();
    CanvasPoint v2 = sortedTriangle.v2();

    if (v1.y == v2.y) {
        fillTopColouredTriangle(v0, v1, v2, colour, window);
    }
    else if (v0.y == v1.y) {
        fillBottomColouredTriangle(v2, v0, v1, colour, window);
    }
    else {
        float splitRatio = (v1.y - v0.y) / (v2.y - v0.y);
        float v3_X = v0.x + splitRatio * (v2.x - v0.x);
        CanvasPoint v3 = {v3_X, v1.y};

        fillTopColouredTriangle(v0, v1, v3, colour, window);
        fillBottomColouredTriangle(v2, v1, v3, colour, window);
    }
}