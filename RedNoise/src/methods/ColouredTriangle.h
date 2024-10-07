#pragma once

#include <DrawingWindow.h>
#include <CanvasPoint.h>
#include <Colour.h>
#include "Triangle.h"

class ColouredTriangle {
public:
    static void fillTopColouredTriangle(CanvasPoint topVertex, CanvasPoint bottomEdge1, CanvasPoint bottomEdge2, Colour colour, DrawingWindow &window);
    static void fillBottomColouredTriangle(CanvasPoint bottomVertex, CanvasPoint topEdge1, CanvasPoint topEdge2, Colour colour, DrawingWindow &window);
    static void fillColouredTriangle(CanvasTriangle triangle, Colour colour, DrawingWindow &window);
};