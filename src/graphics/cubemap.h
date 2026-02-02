#ifndef SDL3_FIRST_CUBEMAP_H
#define SDL3_FIRST_CUBEMAP_H

#include <array>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <SDL3_image/SDL_image.h>

#include "graphics/shader.h"

class Cubemap {
public:
    unsigned int textureID = 0;
    unsigned int VAO = 0;
    unsigned int VBO = 0;

    Cubemap(
        const char* px,
        const char* nx,
        const char* py,
        const char* ny,
        const char* pz,
        const char* nz
    ) {
        load_textures_from_path({ px, nx, py, ny, pz, nz });
        setup_mesh();
    }

    Cubemap(const std::vector<std::vector<uint8_t>> images_data
    ) {
        load_textures_from_data(images_data);
        setup_mesh();
    }



    ~Cubemap() {
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
        glDeleteTextures(1, &textureID);
    }

    void draw(const glm::mat4& view, const glm::mat4& proj, Shader* shader) {
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);

        shader->use();
        shader->setMat4("view", glm::mat4(glm::mat3(view)));
        shader->setMat4("projection", proj);
        shader->setInt("skybox", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

private:
    static constexpr float skybox_vertices[108] = {
        -1,  1, -1,  -1, -1, -1,   1, -1, -1,
         1, -1, -1,   1,  1, -1,  -1,  1, -1,

        -1, -1,  1,  -1, -1, -1,  -1,  1, -1,
        -1,  1, -1,  -1,  1,  1,  -1, -1,  1,

         1, -1, -1,   1, -1,  1,   1,  1,  1,
         1,  1,  1,   1,  1, -1,   1, -1, -1,

        -1, -1,  1,  -1,  1,  1,   1,  1,  1,
         1,  1,  1,   1, -1,  1,  -1, -1,  1,

        -1,  1, -1,   1,  1, -1,   1,  1,  1,
         1,  1,  1,  -1,  1,  1,  -1,  1, -1,

        -1, -1, -1,  -1, -1,  1,   1, -1, -1,
         1, -1, -1,  -1, -1,  1,   1, -1,  1
    };

    void setup_mesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(skybox_vertices),
            skybox_vertices,
            GL_STATIC_DRAW
        );

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE,
            3 * sizeof(float),
            (void*)0
        );

        glBindVertexArray(0);
    }

    void load_textures_from_path(const std::array<const char*, 6>& faces) {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        for (unsigned int i = 0; i < faces.size(); i++) {
            SDL_Surface* surf = IMG_Load(faces[i]);
            if (!surf) {
                SDL_Log("Cubemap load failed: %s", faces[i]);
                continue;
            }

            SDL_Surface* converted =
                SDL_ConvertSurface(surf, SDL_PIXELFORMAT_RGBA32);
            SDL_DestroySurface(surf);

            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGBA8,
                converted->w,
                converted->h,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                converted->pixels
            );

            SDL_DestroySurface(converted);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    void load_textures_from_data(const std::vector<std::vector<uint8_t>>& faces) {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        for (unsigned int i = 0; i < faces.size(); i++) {
            SDL_IOStream* stream = SDL_IOFromConstMem(faces.at(i).data(), faces.at(i).size());
            if (!stream)
                SDL_Log("Cubemap data -> surface conversion failed: %s", std::to_string(i).c_str());


            SDL_Surface* surf = IMG_Load_IO(stream, true);
            if (!surf) {
                SDL_Log("Cubemap surface load after conversion failed: %s", std::to_string(i).c_str());
                continue;
            }

            SDL_Surface* converted =
                SDL_ConvertSurface(surf, SDL_PIXELFORMAT_RGBA32);
            SDL_DestroySurface(surf);

            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGBA8,
                converted->w,
                converted->h,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                converted->pixels
            );

            SDL_DestroySurface(converted);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
};

#endif // SDL3_FIRST_CUBEMAP_H
