#pragma once

#include <DrawingWindow.h>
#include <CanvasPoint.h>
#include <TextureMap.h>

class TexturedTriangle {
public:
    static uint32_t getTextureColour(TexturePoint texturePoint, TextureMap &textureMap);
    static void fillTexturedTriangle(CanvasTriangle triangle, TextureMap &textureMap, DrawingWindow &window);
};