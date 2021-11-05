#pragma once

#pragma region libraries

#include <iostream>
#include <vector>

#if defined(__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "logic.h"
#include "utils.h"

using namespace glm;
#pragma endregion libraries

/*  Inform compiler for existence of initialisation in other file  */
extern const vec3 kColorBlack;
extern const vec3 kColorWhite;

class PieceRenderer
{
private:
    float tileSize_;
    vector<Texture> textures_;
    SpriteRenderer& spriteRenderer_;
public:
    /**
     * @brief Construct a new Piece Renderer object which sets value for data variables
     * 
     * @param tileSize 
     * @param textures 
     * @param spriteRenderer 
     */
    PieceRenderer(float tileSize, const std::vector<Texture>& textures, SpriteRenderer& spriteRenderer)
            : tileSize_(tileSize), textures_(textures), spriteRenderer_(spriteRenderer)  {}
    
    void renderShape(const Piece &piece, float x, float y, float mixCoeff = 0,
                     const vec3& mixColor = kColorBlack, float alphaMultiplier = 1, int startRow = 0) const;
    void renderInitialShape(const Piece& piece, float x, float y) const;
    void renderInitialShapeCentered(const Piece& piece, float x, float y, float width, float height) const;
};

class SpriteRenderer
{
private:
    Shader shader_;
    int vao_;

public:
    /**
 * @brief Construct a new Sprite Renderer object
 * 
 * @param projection matrix

 */
    SpriteRenderer(const mat4 &projection);
    /**
     * @brief render function which as accepts properties for figures
     * 
     * @param texture 
     * @param x 
     * @param y 
     * @param width 
     * @param height 
     * @param mixCoeff 
     * @param mixcolor 
     * @param aplhaMultiply 
     */
    void render(const Texture &texture, float x, float y, float width, float height, float mixCoeff = 0.0f, const vec3 &mixcolor = kColorBlack, float aplhaMultiply = 1);
};

class TextRenderer
{
private:
    vector<Glyph> font_;
    Shader shader_;
    int vbo_, vao_;

public:
    TextRenderer(const mat4 projection, const vector<Glyph> &font);
    void render(const string &text, float x, float y, vec3 color) const;
    void renderCentered(const std::string &text, float x, float y, float width, const vec3 &color) const;
    
    /**
     * @brief Just compute required height and width for fitting
     * 
     * @param text string
     * @return int 
     */
    int computeWidth(const string &text) const;
    int computeHeight(const string &text) const;
};

class BoardRenderer
{
private:
    float tileSize_;
    float x_, y_;
    int nRows_, nCols_;
    
    const std::vector<Texture> tileTextures_;
    
    PieceRenderer& pieceRenderer_, ghostRenderer_;
    SpriteRenderer& spriteRenderer_;
    
    Shader backgroundShader_;
    std::vector<float> verticesBackground_;
    int vaoBackground_;
public:
    /**
     * @brief Construct a new Board Renderer object / master object
     * 
     * @param projection 
     * @param tileSize 
     * @param x 
     * @param y 
     * @param nRows 
     * @param nCols 
     * @param tileTextures 
     * @param spriteRenderer 
     * @param pieceRenderer 
     * @param ghostRenderer 
     */
    BoardRenderer(const mat4& projection, float tileSize, float x, float y,
                  int nRows, int nCols, const std::vector<Texture>& tileTextures,
                  SpriteRenderer& spriteRenderer, PieceRenderer& pieceRenderer, PieceRenderer& ghostRenderer);
    
    /**
     * @brief Individual component rendering
     * 
     */
    void renderBackground() const;
    void renderTiles(const Board& board, float alphaMultiplier = 1) const;
    void renderPiece(const Piece& piece, int row, int col, double lockPercent, double alphaMultiplier = 1) const;
    void renderGhost(const Piece& piece, int ghostRow, int col) const;
    void playLinesClearAnimation(const Board& board, double percentFinished) const;
};
