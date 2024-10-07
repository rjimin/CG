#pragma once

#include <DrawingWindow.h>
#include <CanvasPoint.h>
#include <Colour.h>
#include <TextureMap.h>

class Draw {
public:
    static void drawGreyscale(DrawingWindow &window);
    static void draw2DColour(DrawingWindow &window);
    static void drawLine(CanvasPoint from, CanvasPoint to, Colour colour, DrawingWindow &window);
    static void drawStrokedTriangle(CanvasTriangle triangle, Colour colour, DrawingWindow &window);
    static void drawTexturedLine(CanvasPoint from, CanvasPoint to, TexturePoint fromTP, TexturePoint toTP, TextureMap &textureMap, DrawingWindow &window);
};
