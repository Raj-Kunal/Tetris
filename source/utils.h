#pragma once

#pragma region libraries

#if defined(__APPLE__)
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>


using namespace glm;

#pragma endregion libraries

class Shader
{
private:
    u_int id_;

public:
    /**
     * @brief Construct a new Shader object 
     * 
     * @param sourceVertex initial point
     * @param sourceFragment 
     */
    Shader(const char *sourceVertex, const char *sourceFragment);

    void setFloat(const char *name, float value) const{
        glUniform1f(glGetUniformLocation(id_, name), value);
    }

    /**
     * @brief Set the Mat4 - 4x4 matrix
     * 
     * @param name 
     * @param matrix 
     */
    void setMat4(const char *name, const mat4 &matrix) const{
        // glUniformMatrix4fv() -> set uniform value for passsed variable
        glUniformMatrix4fv(glGetUniformLocation(id_, name), 1, GL_FALSE, value_ptr(matrix));
    }
    
    /**
     * @brief Set the Vec3 object
     * 
     * @param name 
     * @param vec 3d vector
     */
    void setVec3(const char *name, vec3 vec) const{
        // glUniform3f() -> set uniform value for passsed variable
        glUniform3f(glGetUniformLocation(id_, name), vec.x, vec.y, vec.z);
    }
    void setVec2(const char *name, vec2 vec) const{
        // glUniform2f() -> set uniform value for passsed variable
        glUniform2f(glGetUniformLocation(id_, name), vec.x, vec.y);
    }
    void use() const { glUseProgram(id_); }
};

class Texture {
public:
    const u_int width, height;
    Texture() : width(0), height(0) {};
    Texture(u_int format, int width, int height, unsigned char* image);
    
    void bind() const { glBindTexture(GL_TEXTURE_2D, id_); }

private:
    int id_;
};

struct Glyph {
    Texture texture;
    ivec2 bearing;
    GLint64 advance;
};


std::vector<Glyph> loadFont(const std::string &path, unsigned int glyphHeight);
Texture loadRgbaTexture(const std::string &path);