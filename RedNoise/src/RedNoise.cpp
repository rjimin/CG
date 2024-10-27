#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <CanvasPoint.h>
#include <TextureMap.h>

#include "methods/Draw.h"
#include "methods/Constants.h"
#include "methods/Triangle.h"
#include "methods/TexturedTriangle.h"
#include "methods/ColouredTriangle.h"
#include "methods/loadFile.h"

bool isOrbiting = false;

glm::vec3 calculateCenter(const std::vector<ModelTriangle> &triangles) {
    glm::vec3 minCoords = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxCoords = glm::vec3(std::numeric_limits<float>::lowest());

    for (const ModelTriangle &triangle : triangles) {
        for (const glm::vec3 &vertex : triangle.vertices) {
            minCoords.x = std::min(minCoords.x, vertex.x);
            minCoords.y = std::min(minCoords.y, vertex.y);
            minCoords.z = std::min(minCoords.z, vertex.z);

            maxCoords.x = std::max(maxCoords.x, vertex.x);
            maxCoords.y = std::max(maxCoords.y, vertex.y);
            maxCoords.z = std::max(maxCoords.z, vertex.z);
        }
    }
    return (minCoords + maxCoords) / 2.0f;
}

glm::mat3 lookAt(const glm::vec3 &cameraPosition, const glm::vec3 &target) {
    glm::vec3 forward = glm::normalize(cameraPosition - target);
    glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), forward));
    glm::vec3 up = glm::cross(forward, right);
    return glm::mat3(right, up, forward);
}

void draw(DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float focalLength,
          const std::vector<ModelTriangle> &triangles, std::vector<std::vector<float>> &depthBuffer) {

    window.clearPixels();

    for (std::vector<float> &row : depthBuffer) std::fill(row.begin(), row.end(), 0.0f);

    if (isOrbiting) {
        float orbitAngle = glm::radians(0.2f);
        glm::mat3 orbitRotation(
                cos(orbitAngle), 0, sin(orbitAngle),
                0, 1, 0,
                -sin(orbitAngle), 0, cos(orbitAngle)
        );
        
        cameraPosition = glm::normalize(orbitRotation * cameraPosition) * 4.0f;

        glm::vec3 target = calculateCenter(triangles);
        cameraOrientation = lookAt(cameraPosition, target);
    }
    Draw::drawFilledModel(window, cameraPosition, cameraOrientation, focalLength, triangles, depthBuffer);
}

void handleEvent(SDL_Event event, DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation) {
    if (event.type == SDL_KEYDOWN) {
        float translationAmount = 0.1f;
        float rotationAngle = glm::radians(5.0f);

//        if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
//        else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
//        else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
//        else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;

        // Rotation controls
        if (event.key.keysym.sym == SDLK_LEFT) {
            glm::mat3 rotationY(
                    cos(rotationAngle), 0, sin(rotationAngle),
                    0, 1, 0,
                    -sin(rotationAngle), 0, cos(rotationAngle)
            );
            cameraOrientation = rotationY * cameraOrientation;
        }
        else if (event.key.keysym.sym == SDLK_RIGHT) {
            glm::mat3 rotationY(
                    cos(-rotationAngle), 0, sin(-rotationAngle),
                    0, 1, 0,
                    -sin(-rotationAngle), 0, cos(-rotationAngle)
            );
            cameraOrientation = rotationY * cameraOrientation;
        }
        else if (event.key.keysym.sym == SDLK_UP) {
            glm::mat3 rotationX(
                    1, 0, 0,
                    0, cos(rotationAngle), -sin(rotationAngle),
                    0, sin(rotationAngle), cos(rotationAngle)
            );
            cameraOrientation = rotationX * cameraOrientation;
        }
        else if (event.key.keysym.sym == SDLK_DOWN) {
            glm::mat3 rotationX(
                    1, 0, 0,
                    0, cos(-rotationAngle), -sin(-rotationAngle),
                    0, sin(-rotationAngle), cos(-rotationAngle)
            );
            cameraOrientation = rotationX * cameraOrientation;
        }

        else if (event.key.keysym.sym == SDLK_o) isOrbiting = !isOrbiting;
        // Translation controls
        else if (event.key.keysym.sym == SDLK_w) cameraPosition.z -= translationAmount;
        else if (event.key.keysym.sym == SDLK_s) cameraPosition.z += translationAmount;
        else if (event.key.keysym.sym == SDLK_a && cameraPosition.x > - (WIDTH / 2)) cameraPosition.x -= translationAmount;
        else if (event.key.keysym.sym == SDLK_d && cameraPosition.x < WIDTH / 2) cameraPosition.x += translationAmount;
        else if (event.key.keysym.sym == SDLK_q && cameraPosition.y < HEIGHT / 2) cameraPosition.y += translationAmount;
        else if (event.key.keysym.sym == SDLK_e && cameraPosition.y > - (HEIGHT / 2)) cameraPosition.y -= translationAmount;

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

    std::vector<std::vector<float>> depthBuffer(WIDTH, std::vector<float>(HEIGHT, 0.0f));

    loadFile objFile;
    objFile.loadObj();

    glm::vec3 cameraPosition = {0.0, 0.0, 4.0};
    glm::mat3 cameraOrientation = glm::mat3(1.0f);

    float focalLength = 2.0;

//    std::vector<float> result;
//    result = interpolateSingleFloats(2.2, 8.5, 7);
//    for(size_t i=0; i<result.size(); i++) std::cout << result[i] << " ";
//    std::cout << std::endl;

//    CanvasPoint top_left = CanvasPoint(0, 0);
//    CanvasPoint top_right = CanvasPoint(WIDTH, 0);
//    CanvasPoint centre = CanvasPoint(WIDTH / 2,HEIGHT / 2);

    while (true) {
        // We MUST poll for events - otherwise the window will freeze !
        if (window.pollForInputEvents(event)) handleEvent(event, window, cameraPosition, cameraOrientation);
        draw(window, cameraPosition, cameraOrientation, focalLength, objFile.triangles, depthBuffer);
//		drawLine(top_left, centre, colour, window);
//		drawLine(top_right, centre, colour, window);
//		drawLine(CanvasPoint(WIDTH / 2, 0), CanvasPoint(WIDTH / 2, HEIGHT), colour, window);
//		drawLine(CanvasPoint(WIDTH / 3, HEIGHT / 2), CanvasPoint((WIDTH / 3) * 2, HEIGHT / 2), colour, window);
        // Need to render the frame at the end, or nothing actually gets shown on the screen !
        window.renderFrame();
    }
}
