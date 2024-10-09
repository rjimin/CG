#pragma once

#include <DrawingWindow.h>
#include <CanvasPoint.h>
#include <Colour.h>
#include "Triangle.h"

class ColouredTriangle {
public:
    static void fillColouredTriangle(CanvasTriangle triangle, Colour colour, DrawingWindow &window);
};