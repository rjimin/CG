#pragma once

#include <DrawingWindow.h>
#include <CanvasPoint.h>
#include <TextureMap.h>

class TexturedTriangle {
public:
    static void fillTexturedTriangle(CanvasTriangle triangle, TextureMap &textureMap, DrawingWindow &window);
};