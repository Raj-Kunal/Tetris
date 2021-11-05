# pragma region libraries

#include <string>
#include <iostream>
#include <fstream>
#include <vector> 

#include "utils.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_loader.h"

# pragma endregion libraries

// Loads a font from a file
Shader::Shader
{
    // Load the vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);


    // Check for errors
    char infoLog[512];
    int success;

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    // If the compilation failed, print the error log
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // Load the fragment shader
    u_int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    // If the compilation failed, print the error log
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // Link the vertex and fragment shader into a shader program
    u_int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);


    // Check for errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    // If the compilation failed, print the error log
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    // Delete the shaders as they're linked into our program now and no longer
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    id_ = shaderProgram;
}
// Loads a texture from file
Texture loadRgbaTexture(const std::string &filename)
{
    // Load the image
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
    Texture texture(GL_RGBA, width, height, data);
    stbi_image_free(data);
    return texture;
}

//  Loads a texture from file
Texture::Texture(GLenum format, int width, int height, unsigned char *data)
    : format_(format), width_(width), height_(height)
{
    // Create a texture object
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);
    glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0, format_, GL_UNSIGNED_BYTE, data);
    // glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glBindTexture(GL_TEXTURE_2D, 0);
}

// Loads a texture from file
std::vector<Glyph> loadFont(const std::string &filename, int fontSize)
{
    // Load the font
    FT_Library ft;
    FT_Face face;

    // Initialize the FreeType library
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    FT_New_Face(ft, filename.c_str(), 0, &face)
        FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    std::vector<Glyph> glyphs;
    // Load the first 128 characters of ASCII
    for (u_int c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }
        Texture texture(GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);
        Glyph glyph = {texture,
                       glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                       face->glyph->advance.x >> 6};
        glyphs.push_back(glyph);
    }

    // Destroy the FreeType library
        FT_Done_FreeType(ft);
        return glyphs;
}