/**
 * @file master.cpp
 * @brief This is the master implementstion file, which initiate rendering, logic, utilities
 * @version 1.0.0
 * @date 2021
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma region libraries

#include <map>
#include <string>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "logic.h"      // temporary addition
#include <GLFW/glfw3.h> // temporary addition
// #include "render.h" Under Developement

using namespace glm;
#pragma endregion libraries

/**
 * @brief Board visual properties
 * 
 */
const float kTileSize = 32;
const int kBoardNumRows = 20;
const int kBoardNumCols = 10;
const float kBoardWidth = kBoardNumCols * kTileSize;
const float kBoardHeight = kBoardNumRows * kTileSize;
const float kMargin = 10;
const float kHudWidth = 160;
const float kWidth = 3 * kMargin + kBoardWidth + kHudWidth;
const float kHeight = 2 * kMargin + kBoardHeight;
const float kHudX = kMargin;
const float kHudY = kMargin;
const float kBoardX = 2 * kMargin + kHudWidth;
const float kBoardY = kMargin;
const float kHudPieceBoxHeight = 2.5f * kTileSize;
const u_int kFontSize = 18;

/**
 * @brief game properties such as time increment, fps
 * 
 */
const double kGameTimeStep = 0.005;
const double kFps = 30;
const double kSecondsPerFrame = 1.0 / kFps;

Board board(kBoardNumRows, kBoardNumCols);
Tetris *tetris; // Class initialisation

enum GameState
{
    kGameStart,
    kGameRun,
    kGamePaused,
    kGameOver
};
GameState gameState = kGameStart; // Initialise the gameState with zero

/**
 * @brief game properties initialisation as primary state
 * 
 */
bool softDrop = false;
bool moveRight = false;
bool moveLeft = false;
int startLevel = 1;

/**
 * @brief Initiating the GL Window
 * 
 * @return GLFWwindow* instance
 */
GLFWwindow *setupGLContext()
{
    if (!glfwInit())
        return NULL;
    glfwWindowHint(GLFW_RESIZABLE, false);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    GLFWwindow *window = glfwCreateWindow(kWidth, kHeight, "TETRIS", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(true);
    }
    glfwMakeContextCurrent(window);
/**
 * @brief Check whether WIN32, macro is defined
 * 
 */
#if defined(WIN32)
    glewExperimental = GL_TRUE;
    glewInit();
#endif
    return window;
}

/**
 * @brief handles the key calling for gameover, paused, continue
 * 
 * @param window 
 * @param key 
 * @param scancode 
 * @param action either press or release
 * @param mods 
 */
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    switch (gameState)
    {
    case kGameRun:
        // action and only be press or release which is 1 / 0 respectively
        if (action)
        {
            /**
             * @brief Construct a new switch object
             *  for the following things:
             * - rotation
             * - hardDrop
             * - hold
             * 
             * Later, binding the functions to keys ASCII press and release
             */
            switch (key)
            {
            case GLFW_KEY_Z:
                tetris->rotate(Rotation::kLeft);
                break;
            case GLFW_KEY_X:
                tetris->rotate(Rotation::kRight);
                break;
            case GLFW_KEY_SPACE:
                tetris->hardDrop();
                break;
            case GLFW_KEY_C:
                tetris->hold();
                break;
            case GLFW_KEY_LEFT:
                moveLeft = true;
                break;
            case GLFW_KEY_RIGHT:
                moveRight = true;
                break;
            case GLFW_KEY_DOWN:
                softDrop = true;
                break;
            case GLFW_KEY_ESCAPE:
                gameState = kGamePaused;
            }
        }
        else if (!action)
        {
            switch (key)
            {
            case GLFW_KEY_LEFT:
                moveLeft = false;
                break;
            case GLFW_KEY_RIGHT:
                moveRight = false;
                break;
            case GLFW_KEY_DOWN:
                softDrop = false;
            }
        }
        break;
    case kGamePaused:
        if (key == GLFW_KEY_ESCAPE && action)
            gameState = kGameRun;
        else if (key == GLFW_KEY_ENTER && action)
            gameState = kGameStart;
        break;
    case kGameOver:
        if (key == GLFW_KEY_ENTER && action)
            gameState = kGameStart;
        break;
    case kGameStart:
        if (key == GLFW_KEY_ENTER && action)
        {
            moveRight = false;
            moveLeft = false;
            softDrop = false;
            tetris->restart(startLevel);
            gameState = kGameRun;
        }
        else if (key == GLFW_KEY_UP && action)
        {
            startLevel = std::min(15, startLevel + 1);
        }
        else if (key == GLFW_KEY_DOWN && action)
        {
            startLevel = std::max(1, startLevel - 1);
        }
    }
}
/**
 * @brief just checks whether game window is focused or not 
 * 
 * @param window 
 * @param focused 
 */
void windowFocusCallback(GLFWwindow* window, int focused) {
    if (!focused && gameState == kGameRun)
        gameState = kGamePaused;
}

int main(int argc, char const *argv[])
{
    // Returns a initialised GLFWwindow
    GLFWwindow *window = setupGLContext();
    if(!window)
        return 1;
    
    return 0;
}
