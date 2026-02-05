//
// Created by Charlie on 07/01/2026.
//

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_log.h>

class Shader {
public:
    std::string name;
    unsigned int ID;
    unsigned int UBB; // Uniform Block Binding;

    int point_light_count = 0;
    int dir_light_count = 0;

    Shader(const char* vertex_path, const char* fragment_path, const char* name) {
        this->name = name;

        size_t vsize = 0;
        char* vsrc = (char*)SDL_LoadFile(vertex_path, &vsize);
        if (!vsrc) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load %s", vertex_path);
            return;
        }

        size_t fsize = 0;
        char* fsrc = (char*)SDL_LoadFile(fragment_path, &fsize);
        if (!fsrc) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load %s", fragment_path);
            SDL_free(vsrc);
            return;
        }

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vsrc, (GLint*)&vsize);
        glCompileShader(vs);

        GLint success;
        glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[512];
            glGetShaderInfoLog(vs, 512, nullptr, log);
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Vertex compile error: %s", log);
        }

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fsrc, (GLint*)&fsize);
        glCompileShader(fs);

        glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[512];
            glGetShaderInfoLog(fs, 512, nullptr, log);
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fragment compile error: %s", log);
        }

        ID = glCreateProgram();
        glAttachShader(ID, vs);
        glAttachShader(ID, fs);
        glLinkProgram(ID);

        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            char log[512];
            glGetProgramInfoLog(ID, 512, nullptr, log);
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Program link error: %s", log);
        }

        GLuint globalsIndex = glGetUniformBlockIndex(ID, "Globals");
        if (globalsIndex != GL_INVALID_INDEX) {
            glUniformBlockBinding(ID, globalsIndex, 0);
        }

        glDeleteShader(vs);
        glDeleteShader(fs);

        SDL_free(vsrc);
        SDL_free(fsrc);
    }

    void use() {
        glUseProgram(ID);
    }

    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }


    void setVec2(const std::string &name, glm::vec2 value) const {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(value));
    }
    void setVec3(const std::string &name, glm::vec3 value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(value));
    }
    void setVec4(const std::string &name, glm::vec4 value) const {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(value));
    }
    void setMat4(const std::string &name, glm::mat4 value) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value_ptr(value));
    }

};

#endif