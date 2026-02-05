//
// Created by User on 04/02/2026.
//

#ifndef SDL3_FIRST_FRAMEBUFFER_H
#define SDL3_FIRST_FRAMEBUFFER_H
#include <cstddef>
#include <iostream>
#include <glad/glad.h>

class Framebuffer {
private:
    unsigned int width, height;
public:
    unsigned int ID;
    std::vector<Texture> textures;
    std::vector<unsigned int> render_buffers;

    Framebuffer(unsigned int width, unsigned int height) {
        glGenFramebuffers(1, &ID);
        this->width = width; this->height = height;
    }

    int new_texture(int attachment_type = GL_COLOR_ATTACHMENT0, bool increment = true) {
        /*
         * Creates and attaches a 2D texture to the framebuffer. The type of attachment can be passed as args.
         * An attachment of `GL_COLOR_ATTACHMENT0` will assume there will be multiple color attachments and internally indexes the attachment
         * returns the index of the texture inside framebuffer, NOT THE ID of the newly created texture.
         */
        this->use();
        if (attachment_type == GL_COLOR_ATTACHMENT0) {
            if (increment && textures.size() > 0) attachment_type += textures.size();
            textures.emplace_back(width, height);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_type, GL_TEXTURE_2D, textures[textures.size()-1].ID, 0);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
            unbind();
            return textures.size()-1;
        }

        unbind();
        std::cerr << "Framebuffer failed to create new texture" << std::endl;
        return -1;
    }

    int new_render_buffer(int internal_format = GL_DEPTH24_STENCIL8) {
        /*
         * Generates a render buffer then stores it under the framebuffer.
         * Internal format is assumed to be a depth-stencil, passing 0 as an arg avoids this.
         * Returns the index of the render buffer within the framebuffer, NOT THE ID.
         */
        this->use();

        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        render_buffers.emplace_back(rbo);

        if (internal_format != 0) {
            glRenderbufferStorage(GL_RENDERBUFFER, internal_format, width, height);

            if (internal_format == GL_DEPTH24_STENCIL8) {
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
            }
        }

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            unbind();
            return render_buffers.size()-1;
        }
        unbind();
        std::cerr << "Framebuffer failed to create new render buffer" << std::endl;
        return -1;
    }

    void clear() {
        use();
        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void use() {
        glBindFramebuffer(GL_FRAMEBUFFER, ID);
    }

    void use_renderbuffer(int index) {
        glBindRenderbuffer(GL_RENDERBUFFER, render_buffers[index]);
    }

    void use_texture(int index) {
        glBindTexture(GL_TEXTURE_2D, textures[index].ID);
    }

    static void unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

};








#endif //SDL3_FIRST_FRAMEBUFFER_H