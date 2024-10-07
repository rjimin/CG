#pragma once

#include <DrawingWindow.h>
#include <CanvasPoint.h>
#include <TextureMap.h>

class TexturedTriangle {
public:
    static uint32_t getTextureColour(TexturePoint texturePoint, TextureMap &textureMap);
    static void fillTopTexturedTriangle(CanvasPoint topVertex, CanvasPoint bottomEdge1, CanvasPoint bottomEdge2, TextureMap &textureMap, DrawingWindow &window);
    static void fillBottomTexturedTriangle(CanvasPoint bottomVertex, CanvasPoint topEdge1, CanvasPoint topEdge2, TextureMap &textureMap, DrawingWindow &window);
    static void fillTexturedTriangle(CanvasTriangle triangle, TextureMap &textureMap, DrawingWindow &window);
};