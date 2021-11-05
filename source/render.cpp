#pragma region Header

#include "render.h"
#define _USE_MATH_DEFINES
#include <math.h>
using namespace glm;

#pragma endregion Header

#pragma region Constructors

const char *kColoredPrimitiveVertexShader = R"glsl(
# version 330 core

layout (location = 0) in vec2 position;
uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(position, 0, 1);
}

)glsl";

const char *kColoredPrimitiveFragmentShader = R"glsl(
# version 330 core

uniform vec3 inColor;
out vec4 color;

void main() {
    color = vec4(inColor, 1);
}

)glsl";

const char *kTileVertexShader = R"glsl(
# version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;

out vec2 texCoordFragment;

uniform vec2 shift;
uniform vec2 scale = vec2(1, 1);
uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(scale * position + shift, 0, 1);
    texCoordFragment = texCoord;
}
)glsl";

const char *kTileFragmentShader = R"glsl(

# version 330 core

in vec2 texCoordFragment;
out vec4 color;

uniform sampler2D sampler;
uniform vec3 mixColor;
uniform float mixCoeff = 0;
uniform float alphaMultiplier = 1;

void main() {
    color = mix(texture(sampler, texCoordFragment), vec4(mixColor, 1), mixCoeff);
    color.a *= alphaMultiplier;
}

)glsl";

const char *kGlyphVertexShader = R"glsl(

#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;

out vec2 texCoordFragment;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(position, 0, 1);
    texCoordFragment = texCoord;
}

)glsl";

const char *kGlyphFragmentShader = R"glsl(

#version 330 core

in vec2 texCoordFragment;
out vec4 color;

uniform vec3 textColor;
uniform sampler2D glyph;

void main() {
    float alpha = texture(glyph, texCoordFragment).r;
    color = vec4(textColor, alpha);
}

)glsl";

#pragma endregion Constructors

const vec3 kColorBlack(0, 0, 0);
const vec3 kColorWhite(1, 1, 1);

/**
 * @brief Construct a new Sprite Renderer:: Sprite Renderer object
 * 
 * @param projection 
 */
SpriteRenderer::SpriteRenderer(const mat4 &projection) : shader_(kTileVertexShader, kTileFragmentShader)
{
    // Set projection matrix
    float vertices[] = {
        0, 0, 0, 1,
        0, 1, 0, 0,
        1, 0, 1, 1,
        1, 1, 1, 0};

    // create vertex buffer object
    u_int vbo;

    // bind buffer
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // set vertex attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Set projection matrix
    shader_.use();
    shader_.setMat4("projection", projection);
}

/**
 * @brief Destroy the Sprite Renderer:: Sprite Renderer object
 * 
 * @param texture 
 * @param x 
 * @param y 
 * @param width 
 * @param height 
 * @param mixCoeff 
 * @param mixColor 
 * @param alphaMultiplier 
 *//
void SpriteRenderer::render(const Texture &texture, float x, float y, float width, float height,
                            float mixCoeff, const vec3 &mixColor, float alphaMultiplier)
{
    // bind texture
    texture.bind();
    // render textured quad
    shader_.use();
    // set shader uniforms
    shader_.setVec2("shift", vec2(x, y));
    shader_.setVec2("scale", vec2(width, height));
    shader_.setFloat("mixCoeff", mixCoeff);
    shader_.setVec3("mixColor", mixColor);
    // render quad
    shader_.setFloat("alphaMultiplier", alphaMultiplier);
    // render quad
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

/**
 * @brief Constructor
 * 
 * @param piece 
 * @param x 
 * @param y 
 * @param mixCoeff 
 * @param mixColor 
 * @param alphaMultiplier 
 * @param startRow 
 *//
void PieceRenderer::renderShape(const Piece &piece, float x, float y, float mixCoeff, const vec3 &mixColor,
                                float alphaMultiplier, int startRow) const
{
    // render shape
    if (piece.kind() == kNone)
        return;
    // bind texture
    Texture texture = textures_.at(piece.color());

    int index = startRow * piece.bBoxSide();
    auto shape = piece.shape();

    // render tile wise piece
    for (int row = startRow; row < piece.bBoxSide(); ++row)
    {
        for (int col = 0; col < piece.bBoxSide(); ++col)
        {
            if (shape[index] != kEmpty)
            {
                spriteRenderer_.render(texture, x + col * tileSize_, y + row * tileSize_,
                                       tileSize_, tileSize_, mixCoeff, mixColor, alphaMultiplier);
            }
            ++index;
        }
    }
}
/**
 * @brief render a piece
 * 
 * @param piece 
 * @param x 
 * @param y 
 * @param width 
 * @param height 
 *//
void PieceRenderer::renderInitialShapeCentered(const Piece &piece, float x, float y,
                                               float width, float height) const
{
    float pieceWidth = tileSize_ * piece.nCols();
    float pieceHeight = tileSize_ * piece.nRows();
    float xOffset = (width - pieceWidth) / 2;
    float yOffset = (height - pieceHeight) / 2;

    renderInitialShape(piece, x + xOffset, y + yOffset);
}

const vec3 kBackgroundColor(0.1, 0.1, 0.1);
const vec3 kGridColor(0.2, 0.2, 0.2);

/**
 * @brief Construct a new Board Renderer:: Board Renderer object
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
BoardRenderer::BoardRenderer(const mat4 &projection, float tileSize, float x, float y,
                             int nRows, int nCols, const std::vector<Texture> &tileTextures,
                             SpriteRenderer &spriteRenderer, PieceRenderer &pieceRenderer, PieceRenderer &ghostRenderer)
    : tileSize_(tileSize),
      x_(x), y_(y),
      nRows_(nRows), nCols_(nCols),
      tileTextures_(tileTextures),
      pieceRenderer_(pieceRenderer), ghostRenderer_(ghostRenderer), spriteRenderer_(spriteRenderer),
      backgroundShader_(kColoredPrimitiveVertexShader, kColoredPrimitiveFragmentShader)
{
    // Set projection matrix
    backgroundShader_.use();
    backgroundShader_.setMat4("projection", projection);

    // create vertex buffer object
    float width = tileSize_ * nCols_;
    float height = tileSize_ * nRows_;

    // create vertex buffer object
    verticesBackground_ =
        {
            x_, y_,
            x_, y_ + height,
            x_ + width, y_,
            x_ + width, y_ + height};
    float yGrid = y_;
    float xGrid = x_;

    // create vertex buffer object
    for (int row = 0; row < nRows_ + 1; ++row)
    {
        verticesBackground_.push_back(x_);
        verticesBackground_.push_back(yGrid);
        verticesBackground_.push_back(x_ + width);
        verticesBackground_.push_back(yGrid);
        yGrid += tileSize_;
    }
    // create vertex buffer object
    for (int col = 0; col < nCols_ + 1; ++col)
    {
        verticesBackground_.push_back(xGrid);
        verticesBackground_.push_back(y_);
        verticesBackground_.push_back(xGrid);
        verticesBackground_.push_back(y_ + height);
        xGrid += tileSize_;
    }
    u_int vbo;
    // bind buffer
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vaoBackground_);
    glBindVertexArray(vaoBackground_);

    // bind buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verticesBackground_.size() * sizeof(float), verticesBackground_.data(), GL_STATIC_DRAW);

    // set vertex attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}
/**
 * @brief draw the background of the board
 * 
 */
void BoardRenderer::renderBackground() const
{
    // bind texture
    backgroundShader_.use();
    glBindVertexArray(vaoBackground_);

    backgroundShader_.setVec3("inColor", kBackgroundColor);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    // render quad
    backgroundShader_.setVec3("inColor", kGridColor);
    glDrawArrays(GL_TRIANGLE_STRIP, 4, 2* (nRows_ + nCols + 2));
}
/**
 * @brief draw the board
 * 
 * @param board 
 * @param alphaMultiplier 
 */
void BoardRenderer::renderTiles(const Board &board, float alphaMultiplier) const {
    int row, col;
    float y, x;
    float y0 = y_;
    float x0 = x_;
    
    // render tiles
    for (row = 0, y = y0; row < board.nRows; ++row, y += tileSize_) {
        for (col = 0, x = x0; col < board.nCols; ++col, x += tileSize_) {
            TileColor tile = board.tileAt(row, col);
            if (tile == kEmpty)
                continue;
            
            spriteRenderer_.render(tileTextures_.at(tile), x, y,
                                   tileSize_, tileSize_, 0, vec3(), alphaMultiplier);
        }
    }
    
}

/**
 * @brief draw the pieces
 * 
 * @param piece 
 * @param row 
 * @param col 
 * @param lockPercent 
 * @param alphaMultiplier 
 *//
void BoardRenderer::renderPiece(const Piece &piece, int row, int col, double lockPercent,
                                double alphaMultiplier) const {
    int startRow = std::max(0, -row);
    float mixCoeff = 0.5f * sin(M_PI_2 * lockPercent);

    // render ghost piece
    pieceRenderer_.renderShape(piece, x_ + col * tileSize_, y_ + row * tileSize_,
                               mixCoeff, kColorBlack, alphaMultiplier, startRow);
}

/**
 * @brief draw the ghost piece
 * 
 * @param piece 
 * @param ghostRow 
 * @param col 
 *//
void BoardRenderer::renderGhost(const Piece &piece, int ghostRow, int col) const {
    int startRow = std::max(0, -ghostRow);
    ghostRenderer_.renderShape(piece, x_ + col * tileSize_, y_ + ghostRow * tileSize_,
                               0, kColorBlack, 0.7, startRow);
}

/**
 * @brief define line clear animation
 * 
 * @param board 
 * @param percentFinished 
 *//
void BoardRenderer::playLinesClearAnimation(const Board &board, double percentFinished) const {
    double t = 0.3;
    
    vec3 mixColor;
    float mixCoeff;
    
    // render lines
    if (percentFinished < t) {
        double s = sin(M_PI * percentFinished / t);
        mixColor = kColorWhite;
        mixCoeff = 0.8f * s;
    } else {
        mixColor = kBackgroundColor;
        mixCoeff = (percentFinished - t) / (1 - t);
    }

    // render new lines
    for (int row : board.linesToClear()) {
        for (int col = 0; col < nCols_; ++col) {
            float x = x_ + col * tileSize_;
            float y = y_ + row * tileSize_;

            // render tile
            spriteRenderer_.render(tileTextures_.at(board.tileAt(row, col)),
                                    x, y, tileSize_, tileSize_, mixCoeff, mixColor, 1);
        }
    }
}

/**
 * @brief Construct a new Text Renderer:: Text Renderer object
 * 
 * @param projection 
 * @param font 
 */
TextRenderer::TextRenderer(const mat4& projection, const std::vector<Glyph>& font) :
        font_(font), shader_(kGlyphVertexShader, kGlyphFragmentShader) {
    shader_.use();
    shader_.setMat4("projection", projection);
    
    // create vertex buffer object
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    // set vertex attributes
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // set vertex attributes
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (GLvoid*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

/**
 * @brief render text
 * 
 * @param text 
 * @param x 
 * @param y 
 * @param color 
 */
void TextRenderer::render(const std::string& text, float x, float y, vec3 color) const {
    shader_.use();
    shader_.setVec3("textColor", color);
    glBindVertexArray(vao_);
    
    x = std::round(x);
    y = std::round(y);
    
    // render glyphs
    for (char c : text) {
        Glyph glyph = font_.at(c);
        
        float xBbox = x + glyph.bearing.x;
        float yBbox = y + (font_.at('A').bearing.y - glyph.bearing.y);
        
        float width = glyph.texture.width;
        float height = glyph.texture.height;
        
        float vertices[] = {xBbox,// TODO: move to utils yBbox, 0, 0,
                              xBbox, yBbox + height, 0 ,1,
                              xBbox + width, yBbox, 1, 0,
                              xBbox + width, yBbox + height, 1, 1};
        
        // render glyph texture
        glyph.texture.bind();
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        x += glyph.advance;
    }
}

/**
 * @brief Construct a new Score Renderer:: Score Renderer object
 * 
 * @param text 
 * @param x 
 * @param y 
 * @param width 
 * @param color 
 */
void TextRenderer::renderCentered(const std::string& text, float x, float y, float width,
                                  const vec3& color) const {
    float textWidth = computeWidth(text);
    float shift = 0.5f * (width - textWidth);
    render(text, std::round(x + shift), std::round(y), color);
}

/**
 * @brief compute the width of the text
 * 
 * @param text 
 * @return int 
 */
int TextRenderer::computeWidth(const std::string& text) const {
    int width = 0;

    // compute width
    for (auto c = text.begin(); c != text.end() - 1; ++c) {
        width += font_.at(*c).advance;
    }
    width += font_.at(text.back()).texture.width;
    return width;
}

/**
 * @brief Compute the height of the text
 * 
 * @param text 
 * @return int 
 */
int TextRenderer::computeHeight(const std::string& text) const {
    int height = 0;

    // compute height
    for (char c : text) {
        Glyph glyph = font_.at(c);
        int textureHeight = static_cast<int>(glyph.texture.height);
        height = std::max(height, font_.at('H').bearing.y - glyph.bearing.y + textureHeight);
    }
    return height;
}
