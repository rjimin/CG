#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <Colour.h>
#include <TextureMap.h>

#define WIDTH 320
#define HEIGHT 240

CanvasTriangle RandomTriangle() {
    CanvasPoint v0 = {static_cast<float>(rand() % WIDTH), static_cast<float>(rand() % HEIGHT)};
    CanvasPoint v1 = {static_cast<float>(rand() % WIDTH), static_cast<float>(rand() % HEIGHT)};
    CanvasPoint v2 = {static_cast<float>(rand() % WIDTH), static_cast<float>(rand() % HEIGHT)};

    return CanvasTriangle{v0, v1, v2};
}

CanvasTriangle sortVertices(CanvasTriangle triangle) {
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

uint32_t getTextureColour(TexturePoint texturePoint, TextureMap &textureMap) {
    int texX = round(texturePoint.x * (textureMap.width - 1));
    int texY = round(texturePoint.y * (textureMap.height - 1));

    int index = texY * textureMap.width + texX;

    return textureMap.pixels[index];
}

// Interpolation =======================================================================================================================================================

std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues) {
    std::vector<float> result;
    float step = (to - from) / (numberOfValues - 1);
    for (int i = 0; i < numberOfValues; ++i) {
        result.push_back(from + step * i);
    }
    return result;
}

std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfValues) {
    std::vector<glm::vec3> result;

    float stepX = (to.x - from.x) / (numberOfValues - 1);
    float stepY = (to.y - from.y) / (numberOfValues - 1);
    float stepZ = (to.z - from.z) / (numberOfValues - 1);

    for (int i = 0; i < numberOfValues; ++i) {
        glm::vec3 interpolatedVec3;
        interpolatedVec3.x = from.x + stepX * i;
        interpolatedVec3.y = from.y + stepY * i;
        interpolatedVec3.z = from.z + stepZ * i;

        result.push_back(interpolatedVec3);
    }
    return result;
}

std::vector<CanvasPoint> interpolateCanvasPoints(CanvasPoint from, CanvasPoint to, float numberOfValues) {
    std::vector<CanvasPoint> result;

    float stepX = (to.x - from.x) / (numberOfValues - 1);
    float stepY = (to.y - from.y) / (numberOfValues - 1);

    for (int i = 0; i < numberOfValues; ++i) {
        CanvasPoint canvasPoints;
        canvasPoints.x = from.x + stepX * i;
        canvasPoints.y = from.y + stepY * i;


        result.push_back(canvasPoints);
    }
    return result;
}

std::vector<TexturePoint> interpolateTexturePoints(TexturePoint from, TexturePoint to, float numberOfValues) {
    std::vector<TexturePoint> result;

    float stepX = (to.x - from.x) / (numberOfValues - 1);
    float stepY = (to.y - from.y) / (numberOfValues - 1);

    for (int i = 0; i < numberOfValues; ++i) {
        TexturePoint texturePoint;
        texturePoint.x = from.x + stepX * i;
        texturePoint.y = from.y + stepY * i;

        result.push_back(texturePoint);
    }
    return result;
}

// Draw ================================================================================================================================================================

void drawGreyscale(DrawingWindow &window) {
    window.clearPixels();
    std::vector<float> greyValues = interpolateSingleFloats(255, 0, window.width);
    for (size_t y = 0; y < window.height; y++) {
        for (size_t x = 0; x < window.width; x++) {
            auto greyValue = greyValues[x];
            uint32_t colour = (255 << 24) + (int(greyValue) << 16) + (int(greyValue) << 8) + int(greyValue);
            window.setPixelColour(x, y, colour);
        }
    }
}

void draw2DColour(DrawingWindow &window) {
    window.clearPixels();
    glm::vec3 topLeft(255, 0, 0);        // red
    glm::vec3 topRight(0, 0, 255);       // blue
    glm::vec3 bottomRight(0, 255, 0);    // green
    glm::vec3 bottomLeft(255, 255, 0);   // yellow

    std::vector<glm::vec3> firstCol = interpolateThreeElementValues(topLeft, bottomLeft, window.height);
    std::vector<glm::vec3> lastCol = interpolateThreeElementValues(topRight, bottomRight, window.height);

    for (size_t y = 0; y < window.height; ++y) {
        for (size_t x = 0; x < window.width; ++x) {
            std::vector<glm::vec3> eachRow = interpolateThreeElementValues(firstCol[y], lastCol[y], window.width);
            glm::vec3 pixel = eachRow[x];
            uint32_t colour = (255 << 24) + (uint32_t(pixel[0]) << 16) + (uint32_t(pixel[1]) << 8) + uint32_t(pixel[2]);
            window.setPixelColour(x, y, colour);
        }
    }
}

void drawLine(CanvasPoint from, CanvasPoint to, Colour colour, DrawingWindow &window) {
    float xDiff = to.x -from.x;
    float yDiff = to.y -from.y;

    float numberOfSteps = fmax(abs(xDiff), abs(yDiff));

    float xStepSize = xDiff / numberOfSteps;
    float yStepSize = yDiff / numberOfSteps;

    uint32_t lineColour = (255 << 24) + (uint32_t(colour.red) << 16) + (uint32_t(colour.green) << 8) + uint32_t(colour.blue);

    for (float i = 0.0; i < numberOfSteps; ++i) {
        float x = from.x + (xStepSize * i);
        float y = from.y + (yStepSize * i);
        window.setPixelColour(round(x), round(y), lineColour);
    }
}

void drawStrokedTriangle(CanvasTriangle triangle, Colour colour, DrawingWindow &window){
    drawLine(triangle.v0(), triangle.v1(), colour, window);
    drawLine(triangle.v1(), triangle.v2(), colour, window);
    drawLine(triangle.v2(), triangle.v0(), colour, window);
}

void drawTexturedLine(CanvasPoint from, CanvasPoint to, TexturePoint fromTP, TexturePoint toTP, TextureMap &textureMap, DrawingWindow &window) {
    float xDiff = to.x -from.x;
    float yDiff = to.y -from.y;

    float numberOfSteps = fmax(abs(xDiff), abs(yDiff));

    std::vector<CanvasPoint> canvasPoints = interpolateCanvasPoints(from, to, numberOfSteps);

    std::vector<TexturePoint> texturePoints = interpolateTexturePoints(fromTP, toTP, numberOfSteps);

    for (float i = 0.0; i < canvasPoints.size(); ++i) {
        CanvasPoint canvasPoint = canvasPoints[i];
        TexturePoint texturePoint = texturePoints[i];

        uint32_t colour = getTextureColour(texturePoint, textureMap);

        window.setPixelColour(round(canvasPoint.x), round(canvasPoint.y), colour);
    }
}

// Fill the Coloured Triangle ==========================================================================================================================================

void fillTopColouredTriangle(CanvasPoint topVertex, CanvasPoint bottomEdge1, CanvasPoint bottomEdge2, Colour colour, DrawingWindow &window) {
    float stepX1 = (bottomEdge1.x - topVertex.x) / (bottomEdge1.y - topVertex.y);
    float stepX2 = (bottomEdge2.x - topVertex.x) / (bottomEdge2.y - topVertex.y);

    float currentX1 = topVertex.x;
    float currentX2 = topVertex.x;

    for (float y = topVertex.y; y <= bottomEdge1.y; ++y) {
        CanvasPoint from = {currentX1, y};
        CanvasPoint to = {currentX2, y};

        drawLine(from, to, colour, window);

        currentX1 += stepX1;
        currentX2 += stepX2;
    }
}

void fillBottomColouredTriangle(CanvasPoint bottomVertex, CanvasPoint topEdge1, CanvasPoint topEdge2, Colour colour, DrawingWindow &window) {
    float stepX1 = (bottomVertex.x - topEdge1.x) / (bottomVertex.y - topEdge1.y);
    float stepX2 = (bottomVertex.x - topEdge2.x) / (bottomVertex.y - topEdge2.y);

    float currentX1 = topEdge1.x;
    float currentX2 = topEdge2.x;

    for (float y = topEdge1.y; y <= bottomVertex.y; ++y) {
        CanvasPoint from = {currentX1, y};
        CanvasPoint to = {currentX2, y};

        drawLine(from, to, colour, window);

        currentX1 += stepX1;
        currentX2 += stepX2;
    }
}

void fillColouredTriangle(CanvasTriangle triangle, Colour colour, DrawingWindow &window) {
    CanvasTriangle sortedTriangle = sortVertices(triangle);

    CanvasPoint v0 = sortedTriangle.v0();
    CanvasPoint v1 = sortedTriangle.v1();
    CanvasPoint v2 = sortedTriangle.v2();

    if (v1.y == v2.y) {
        fillTopColouredTriangle(v0, v1, v2, colour, window);
    }
    else if (v0.y == v1.y) {
        fillBottomColouredTriangle(v2, v0, v1, colour, window);
    }
    else {
        float splitRatio = (v1.y - v0.y) / (v2.y - v0.y);
        float v3_X = v0.x + splitRatio * (v2.x - v0.x);
        CanvasPoint v3 = {v3_X, v1.y};

        fillTopColouredTriangle(v0, v1, v3, colour, window);
        fillBottomColouredTriangle(v2, v1, v3, colour, window);
    }
}

// Fill the Textured Triangle ==========================================================================================================================================

void fillTopTexturedTriangle(CanvasPoint topVertex, CanvasPoint bottomEdge1, CanvasPoint bottomEdge2, TextureMap &textureMap, DrawingWindow &window) {
    float stepX1 = (bottomEdge1.x - topVertex.x) / (bottomEdge1.y - topVertex.y);
    float stepX2 = (bottomEdge2.x - topVertex.x) / (bottomEdge2.y - topVertex.y);

    float currentX1 = topVertex.x;
    float currentX2 = topVertex.x;

    for (float y = topVertex.y; y <= bottomEdge1.y; ++y) {
        CanvasPoint from = {currentX1, y};
        CanvasPoint to = {currentX2, y};

        float disRatio = (y - topVertex.y) / (bottomEdge1.y - topVertex.y);
        TexturePoint fromTP = {topVertex.texturePoint.x + disRatio * (bottomEdge1.texturePoint.x - topVertex.texturePoint.x),
                               topVertex.texturePoint.y + disRatio * (bottomEdge1.texturePoint.y - topVertex.texturePoint.y)};
        TexturePoint toTP = {topVertex.texturePoint.x + disRatio * (bottomEdge2.texturePoint.x - topVertex.texturePoint.x),
                             topVertex.texturePoint.y + disRatio * (bottomEdge2.texturePoint.y - topVertex.texturePoint.y)};

        drawTexturedLine(from, to, fromTP, toTP, textureMap, window);

        currentX1 += stepX1;
        currentX2 += stepX2;
    }
}

void fillBottomTexturedTriangle(CanvasPoint bottomVertex, CanvasPoint topEdge1, CanvasPoint topEdge2, TextureMap &textureMap, DrawingWindow &window) {
    float stepX1 = (bottomVertex.x - topEdge1.x) / (bottomVertex.y - topEdge1.y);
    float stepX2 = (bottomVertex.x - topEdge2.x) / (bottomVertex.y - topEdge2.y);

    float currentX1 = topEdge1.x;
    float currentX2 = topEdge2.x;

    for (float y = topEdge1.y; y <= bottomVertex.y; ++y) {
        CanvasPoint from = {currentX1, y};
        CanvasPoint to = {currentX2, y};

        float disRatio = (y - topEdge1.y) / (bottomVertex.y - topEdge1.y);
        TexturePoint fromTP = {topEdge1.texturePoint.x + disRatio * (bottomVertex.texturePoint.x - topEdge1.texturePoint.x),
                               topEdge1.texturePoint.y + disRatio * (bottomVertex.texturePoint.y - topEdge1.texturePoint.y)};
        TexturePoint toTP = {topEdge2.texturePoint.x + disRatio * (bottomVertex.texturePoint.x - topEdge2.texturePoint.x),
                             topEdge2.texturePoint.y + disRatio * (bottomVertex.texturePoint.y - topEdge2.texturePoint.y)};

        drawTexturedLine(from, to, fromTP, toTP, textureMap, window);

        currentX1 += stepX1;
        currentX2 += stepX2;
    }
}

void fillTexturedTriangle(CanvasTriangle triangle, TextureMap &textureMap, DrawingWindow &window) {
    CanvasTriangle sortedTriangle = sortVertices(triangle);

    CanvasPoint v0 = sortedTriangle.v0();
    CanvasPoint v1 = sortedTriangle.v1();
    CanvasPoint v2 = sortedTriangle.v2();

    if (v1.y == v2.y) {
        fillTopTexturedTriangle(v0, v1, v2, textureMap, window);
    }
    else if (v0.y == v1.y) {
        fillBottomTexturedTriangle(v2, v0, v1, textureMap, window);
    }
    else {
        float splitRatio = (v1.y - v0.y) / (v2.y - v0.y);
        CanvasPoint v3 = {v0.x + splitRatio * (v2.x - v0.x), v1.y};
        v3.texturePoint.x = v0.texturePoint.x + splitRatio * (v2.texturePoint.x - v0.texturePoint.x);
        v3.texturePoint.y = v0.texturePoint.y + splitRatio * (v2.texturePoint.y - v0.texturePoint.y);

        fillTopTexturedTriangle(v0, v1, v3, textureMap, window);
        fillBottomTexturedTriangle(v2, v1, v3, textureMap, window);
    }
}

// Keypress ============================================================================================================================================================

void handleEvent(SDL_Event event, DrawingWindow &window) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
        else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
        else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
        else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
        else if (event.key.keysym.sym == SDLK_u) {
            drawStrokedTriangle(RandomTriangle(), {rand()%256, rand()%256, rand()%256}, window);
        }
        else if (event.key.keysym.sym == SDLK_f) {
            CanvasTriangle triangle = RandomTriangle();

            drawStrokedTriangle(triangle, {255, 255, 255}, window);
            fillColouredTriangle(triangle, {rand()%256, rand()%256, rand()%256}, window);
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

            drawStrokedTriangle(triangle, {255, 255, 255}, window);
            fillTexturedTriangle(triangle, textureMap, window);
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
