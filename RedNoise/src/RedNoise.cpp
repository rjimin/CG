#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <CanvasPoint.h>
#include <Colour.h>
#include <TextureMap.h>

#include "methods/Draw.h"
#include "methods/Constants.h"
#include "methods/Triangle.h"
#include "methods/TexturedTriangle.h"
#include "methods/ColouredTriangle.h"

void handleEvent(SDL_Event event, DrawingWindow &window) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
        else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
        else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
        else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
        else if (event.key.keysym.sym == SDLK_u) {
            Draw::drawStrokedTriangle(Triangle::RandomTriangle(), {rand()%256, rand()%256, rand()%256}, window);
        }
        else if (event.key.keysym.sym == SDLK_f) {
            CanvasTriangle triangle = Triangle::RandomTriangle();

            Draw::drawStrokedTriangle(triangle, {255, 255, 255}, window);
            ColouredTriangle::fillColouredTriangle(triangle, {rand()%256, rand()%256, rand()%256}, window);
        }
        else if (event.key.keysym.sym == SDLK_t) {
            TextureMap textureMap("texture.ppm");

            CanvasPoint v0 = {160, 10};
            CanvasPoint v1 = {300, 230};
            CanvasPoint v2 = {10, 150};

            v0.texturePoint = TexturePoint(195.0f / textureMap.width, 5.0f / textureMap.height);
            v1.texturePoint = TexturePoint(395.0f / textureMap.width, 380.0f / textureMap.height);
            v2.texturePoint = TexturePoint(65.0f / textureMap.width, 330.0f / textureMap.height);

            CanvasTriangle triangle = {v0, v1, v2};

            TexturedTriangle::fillTexturedTriangle(triangle, textureMap, window);
            Draw::drawStrokedTriangle(triangle, {255, 255, 255}, window);
        }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN) {
        window.savePPM("output.ppm");
        window.saveBMP("output.bmp");
    }
}

int main(int argc, char *argv[]) {
    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
    SDL_Event event;

//    std::vector<float> result;
//    result = interpolateSingleFloats(2.2, 8.5, 7);
//    for(size_t i=0; i<result.size(); i++) std::cout << result[i] << " ";
//    std::cout << std::endl;

//    CanvasPoint top_left = CanvasPoint(0, 0);
//    CanvasPoint top_right = CanvasPoint(WIDTH, 0);
//    CanvasPoint centre = CanvasPoint(WIDTH / 2,HEIGHT / 2);

    while (true) {
        // We MUST poll for events - otherwise the window will freeze !
        if (window.pollForInputEvents(event)) handleEvent(event, window);
//		drawLine(top_left, centre, colour, window);
//		drawLine(top_right, centre, colour, window);
//		drawLine(CanvasPoint(WIDTH / 2, 0), CanvasPoint(WIDTH / 2, HEIGHT), colour, window);
//		drawLine(CanvasPoint(WIDTH / 3, HEIGHT / 2), CanvasPoint((WIDTH / 3) * 2, HEIGHT / 2), colour, window);
        // Need to render the frame at the end, or nothing actually gets shown on the screen !
        window.renderFrame();
    }
}
