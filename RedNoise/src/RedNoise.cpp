#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <CanvasPoint.h>
#include <TextureMap.h>
#include <RayTriangleIntersection.h>

#include "methods/Draw.h"
#include "methods/Constants.h"
#include "methods/Triangle.h"
#include "methods/TexturedTriangle.h"
#include "methods/ColouredTriangle.h"
#include "methods/LoadFile.h"

bool isOrbiting = false;
bool clearScreen = false;

enum RenderMode {
    IDLE,
    RASTERISED,
    WIREFRAME,
    RAY_TRACED
};

RenderMode currentRenderMode = RASTERISED;

glm::mat3 lookAt(const glm::vec3 &cameraPosition, const glm::vec3 &target) {
    glm::vec3 forward = glm::normalize(cameraPosition - target);
    glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), forward));
    glm::vec3 up = glm::cross(forward, right);
    return glm::mat3(right, up, forward);
}

void orbit(DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float focalLength,
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

        glm::vec3 target = Triangle::calculateCenter(triangles);
        cameraOrientation = lookAt(cameraPosition, target);
    }
    Draw::drawRasterisedScene(window, cameraPosition, cameraOrientation, focalLength, triangles, depthBuffer);
}

void renderScene(DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float focalLength,
                 const std::vector<ModelTriangle> &triangles, std::vector<std::vector<float>> &depthBuffer, const glm::vec3 &lightSource,
                 std::unordered_map<int, glm::vec3> &vertexNormalMap, std::unordered_map<int, std::string> materialMap) {

    if (clearScreen) {
        window.clearPixels();
        clearScreen = false;
        return;
    }

    switch (currentRenderMode) {
        case RASTERISED:
            Draw::drawRasterisedScene(window, cameraPosition, cameraOrientation, focalLength, triangles, depthBuffer);
            break;
        case WIREFRAME:
            Draw::drawWireframe(window, cameraPosition, cameraOrientation, focalLength, triangles, depthBuffer);
            break;
        case RAY_TRACED:
            Draw::drawRayTracedScene(window, cameraPosition, cameraOrientation, focalLength, triangles, lightSource, vertexNormalMap, materialMap);
            break;
        case IDLE:
        default:
            break;
    }
}

void handleEvent(SDL_Event event, DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, glm::vec3 &lightSource) {
    if (event.type == SDL_KEYDOWN) {
        float translationAmount = 0.05f;
        float rotationAngle = glm::radians(2.0f);

//        if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
//        else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
//        else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
//        else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;

        // Rotation controls
        if (event.key.keysym.sym == SDLK_RIGHT) {
            glm::mat3 rotationY(
                    cos(rotationAngle), 0, sin(rotationAngle),
                    0, 1, 0,
                    -sin(rotationAngle), 0, cos(rotationAngle)
            );
            cameraOrientation = rotationY * cameraOrientation;
        }
        else if (event.key.keysym.sym == SDLK_LEFT) {
            glm::mat3 rotationY(
                    cos(-rotationAngle), 0, sin(-rotationAngle),
                    0, 1, 0,
                    -sin(-rotationAngle), 0, cos(-rotationAngle)
            );
            cameraOrientation = rotationY * cameraOrientation;
        }
        else if (event.key.keysym.sym == SDLK_DOWN) {
            glm::mat3 rotationX(
                    1, 0, 0,
                    0, cos(rotationAngle), -sin(rotationAngle),
                    0, sin(rotationAngle), cos(rotationAngle)
            );
            cameraOrientation = rotationX * cameraOrientation;
        }
        else if (event.key.keysym.sym == SDLK_UP) {
            glm::mat3 rotationX(
                    1, 0, 0,
                    0, cos(-rotationAngle), -sin(-rotationAngle),
                    0, sin(-rotationAngle), cos(-rotationAngle)
            );
            cameraOrientation = rotationX * cameraOrientation;
        }

        else if (event.key.keysym.sym == SDLK_o) isOrbiting = !isOrbiting;

        // Translation controls
        else if (event.key.keysym.sym == SDLK_w && cameraPosition.y < HEIGHT / 2) cameraPosition.y += translationAmount;
        else if (event.key.keysym.sym == SDLK_s && cameraPosition.y > - (HEIGHT / 2)) cameraPosition.y -= translationAmount;
        else if (event.key.keysym.sym == SDLK_a && cameraPosition.x > - (WIDTH / 2)) cameraPosition.x -= translationAmount;
        else if (event.key.keysym.sym == SDLK_d && cameraPosition.x < WIDTH / 2) cameraPosition.x += translationAmount;
        else if (event.key.keysym.sym == SDLK_q) cameraPosition.z -= translationAmount;
        else if (event.key.keysym.sym == SDLK_e) cameraPosition.z += translationAmount;

        // Light controls
        if (event.key.keysym.sym == SDLK_i) lightSource.y += translationAmount;
        else if (event.key.keysym.sym == SDLK_k) lightSource.y -= translationAmount;
        else if (event.key.keysym.sym == SDLK_j) lightSource.x -= translationAmount;
        else if (event.key.keysym.sym == SDLK_l) lightSource.x += translationAmount;
        else if (event.key.keysym.sym == SDLK_u) lightSource.z -= translationAmount;
        else if (event.key.keysym.sym == SDLK_o) lightSource.z += translationAmount;

        // 2D Triangle
        else if (event.key.keysym.sym == SDLK_g) {
            Draw::drawStrokedTriangle(Triangle::RandomTriangle(), {rand()%256, rand()%256, rand()%256}, window);
        }
        else if (event.key.keysym.sym == SDLK_f) {
            CanvasTriangle triangle = Triangle::RandomTriangle();

            ColouredTriangle::fillColouredTriangle(triangle, {rand()%256, rand()%256, rand()%256}, window);
            Draw::drawStrokedTriangle(triangle, {255, 255, 255}, window);
        }
        else if (event.key.keysym.sym == SDLK_t) {
            TextureMap textureMap("models/texture.ppm");

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

        // Render Mode controls
        else if (event.key.keysym.sym == SDLK_v) {
            currentRenderMode = IDLE;
            clearScreen = true;
        }
        else if (event.key.keysym.sym == SDLK_b) currentRenderMode = WIREFRAME;
        else if (event.key.keysym.sym == SDLK_n) currentRenderMode = RASTERISED;
        else if (event.key.keysym.sym == SDLK_m) currentRenderMode = RAY_TRACED;
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

    LoadFile objFile;
    objFile.loadObj();

    glm::vec3 cameraPosition = {0.0, 0.0, 4.0};
    glm::mat3 cameraOrientation = glm::mat3(1.0f);

    float focalLength = 2.0;

    glm::vec3 lightSource = {0.0, 0.7, 0.0};

    while (true) {
        // We MUST poll for events - otherwise the window will freeze !
        if (window.pollForInputEvents(event)) handleEvent(event, window, cameraPosition, cameraOrientation, lightSource);

        renderScene(window, cameraPosition, cameraOrientation, focalLength, objFile.triangles, depthBuffer, lightSource, objFile.vertexNormalMap, objFile.materialMap);

        if (isOrbiting) orbit(window, cameraPosition, cameraOrientation, focalLength, objFile.triangles, depthBuffer);
        // Need to render the frame at the end, or nothing actually gets shown on the screen !
        window.renderFrame();
    }
}
