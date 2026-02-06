//
// Created by User on 04/02/2026.
//

#ifndef SDL3_FIRST_FRAMEBUFFER_H
#define SDL3_FIRST_FRAMEBUFFER_H
#include <cstddef>
#include <iostream>
#include <glad/glad.h>
#include "texture.h"

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

    static Framebuffer init_gbuffer(unsigned int width, unsigned int height) {
        Framebuffer fb(width, height);
        fb.use();

        // --- Position buffer (world space) ---
        fb.textures.emplace_back(
            width,
            height,
            GL_RGB16F,   // internal
            GL_RGB,      // format
            GL_FLOAT     // type
        );
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            fb.textures.back().ID,
            0
        );

        // --- Normal buffer ---
        fb.textures.emplace_back(
            width,
            height,
            GL_RGB16F,
            GL_RGB,
            GL_FLOAT
        );
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT1,
            GL_TEXTURE_2D,
            fb.textures.back().ID,
            0
        );

        // --- Albedo + Specular ---
        fb.textures.emplace_back(
            width,
            height,
            GL_RGBA8,
            GL_RGBA,
            GL_UNSIGNED_BYTE
        );
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT2,
            GL_TEXTURE_2D,
            fb.textures.back().ID,
            0
        );

        // --- Enable MRT ---
        const GLenum attachments[3] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2
        };
        glDrawBuffers(3, attachments);

        // --- Depth buffer ---
        GLuint rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH_COMPONENT24,
            width,
            height
        );
        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER,
            rbo
        );
        fb.render_buffers.push_back(rbo);

        // --- Validate ---
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "GBuffer framebuffer incomplete" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return fb;
    }

    int new_texture(int internal_format = GL_SRGB_ALPHA, int attachment_type = GL_COLOR_ATTACHMENT0, int format_type = GL_UNSIGNED_BYTE, bool increment = true) {
        /*
         * Creates and attaches a 2D texture to the framebuffer. The type of attachment can be passed as args.
         * An attachment of `GL_COLOR_ATTACHMENT0` will assume there will be multiple color attachments and internally indexes the attachment
         * returns the index of the texture inside framebuffer, NOT THE ID of the newly created texture.
         */
        this->use();
        if (attachment_type == GL_COLOR_ATTACHMENT0) {
            if (increment && textures.size() > 0) attachment_type += textures.size();
            textures.emplace_back(width, height, internal_format, format_type);
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
            } else if (internal_format == GL_DEPTH_COMPONENT) {
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
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
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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