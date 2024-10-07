#include <CanvasTriangle.h>
#include <Utils.h>
#include <vector>
#include <CanvasPoint.h>
#include "Constants.h"
#include "Triangle.h"

CanvasTriangle Triangle::RandomTriangle() {
    CanvasPoint v0 = {static_cast<float>(rand() % WIDTH), static_cast<float>(rand() % HEIGHT)};
    CanvasPoint v1 = {static_cast<float>(rand() % WIDTH), static_cast<float>(rand() % HEIGHT)};
    CanvasPoint v2 = {static_cast<float>(rand() % WIDTH), static_cast<float>(rand() % HEIGHT)};

    return CanvasTriangle{v0, v1, v2};
}

CanvasTriangle Triangle::sortVertices(CanvasTriangle triangle) {
    CanvasPoint v0 = triangle.v0();
    CanvasPoint v1 = triangle.v1();
    CanvasPoint v2 = triangle.v2();

    // v0: top vertex(has the smallest y-coord),
    // v1: middle vertex,
    // v2: bottom vertex(has the largest y-coord)
    if (v0.y > v1.y) std::swap(v0, v1);
    if (v0.y > v2.y) std::swap(v0, v2);
    if (v1.y > v2.y) std::swap(v1, v2);

    return CanvasTriangle(v0, v1, v2);
}