#pragma once

#include <CanvasTriangle.h>

class Triangle {
public:
    static CanvasTriangle RandomTriangle();
    static CanvasTriangle sortVertices(CanvasTriangle triangle);
};