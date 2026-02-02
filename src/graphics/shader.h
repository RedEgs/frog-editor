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

    int point_light_count = 0;
    int dir_light_count = 0;

    Shader(const char *vertex_path, const char *fragment_path, const char* name) {
        this->name = name;

        size_t vsize;
        char* vertex_shader_source = static_cast<char*>(SDL_LoadFile(vertex_path, &vsize));
        if (!vertex_shader_source) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load %s", vertex_path);

        size_t fsize;
        char* frag_shader_source = static_cast<char*>(SDL_LoadFile(fragment_path, &fsize));
        if (!frag_shader_source) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load %s", fragment_path);

        unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, (const char**)&vertex_shader_source, NULL);
        glCompileShader(vertex_shader);

        int success;
        char infoLog[512];

        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
        glCompileShader(vertex_shader);

        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Vertex shader failed to compile: %s", infoLog);
        }

        unsigned int frag_shader;
        frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag_shader, 1, &frag_shader_source, NULL);
        glCompileShader(frag_shader);

        glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(frag_shader, 512, NULL, infoLog);
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fragment shader failed to compile: %s", infoLog);
        }

        unsigned int shader_program;
        shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, frag_shader);
        glLinkProgram(shader_program);

        glDeleteShader(vertex_shader);
        glDeleteShader(frag_shader);

        ID = shader_program;
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