//
// Created by Charlie on 07/01/2026.
//

#ifndef SDL3_FIRST_TEXTURE_H
#define SDL3_FIRST_TEXTURE_H
#include <cstddef>
#include <iostream>
#include <string>
#include <glad/glad.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>


class Texture {
public:
    unsigned int ID;
    std::string type;
    std::string path;

    Texture(const char* texture_path, const std::string& type_name)
        : type(type_name), path(texture_path)
    {
        SDL_Surface *texture_surface = IMG_Load(texture_path);
        if (!texture_surface) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load texture (%s): %s", texture_path, SDL_GetError());
        }

        std::cout << "Initialised texture ";
        std::cout << static_cast<std::string>(texture_path);
        std::cout << " Of Type ";
        std::cout << type << std::endl;

        SDL_Surface *converted_texure = SDL_ConvertSurface(texture_surface, SDL_PIXELFORMAT_RGBA32);
        if (!converted_texure) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to convert texture (%s): %s", texture_path, SDL_GetError());
        }

        initialise_GL_texture();
        if (type_name == "diffuse") {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, converted_texure->w, converted_texure->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,converted_texure->pixels);
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, converted_texure->w, converted_texure->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,converted_texure->pixels);
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        SDL_free(texture_surface);
        SDL_free(converted_texure);
    }

    Texture() {
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        unsigned char whitePixel[] = {100, 100, 100, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }


    void use() const {
        glBindTexture(GL_TEXTURE_2D, ID);
    }
private:
    void initialise_GL_texture() {
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }



};


#endif //SDL3_FIRST_TEXTURE_H