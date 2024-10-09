#pragma once

#include <DrawingWindow.h>
#include <CanvasPoint.h>
#include <Colour.h>
#include "Triangle.h"

class ColouredTriangleWithDepth {
public:
    static void fillColouredTriangle(CanvasTriangle triangle, Colour colour, DrawingWindow &window, std::vector<std::vector<float>> &depthBuffer);
};
