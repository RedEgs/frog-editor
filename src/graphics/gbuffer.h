//
// Created by User on 06/02/2026.
//

#ifndef SDL3_FIRST_GBUFFER_H
#define SDL3_FIRST_GBUFFER_H
#include <vector>

#include "quad.h"
#include "shader.h"
#include "texture.h"
#include "../core/scenemanager.h"


class GBuffer {
private:
    unsigned int width, height;

    void clear() {
        use();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void use() {
        glBindFramebuffer(GL_FRAMEBUFFER, ID);
    }

    void use_renderbuffer(int index) {
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    }

    void use_texture(int index) {
        glBindTexture(GL_TEXTURE_2D, textures[index].ID);
    }
public:
    unsigned int ID;
    std::vector<Texture> textures;
    unsigned int RBO;

    GBuffer(unsigned int width, unsigned int height) {
        glGenFramebuffers(1, &ID);
        this->width = width; this->height = height;

        use();

        // --- Position buffer (world space) ---
        textures.emplace_back(
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
            textures.back().ID,
            0
        );

        // --- Normal buffer ---
        textures.emplace_back(
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
            textures.back().ID,
            0
        );

        // --- Albedo + Specular ---
        textures.emplace_back(
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
            textures.back().ID,
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
        unsigned int rbo;
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
        RBO = rbo;

        // --- Validate ---
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "GBuffer framebuffer incomplete" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void geometry_pass(Shader *gpass_shader, SceneManager *scene_manager) {
        /*
         * Performs exclusively the geometry pass, must be used before the light pass.
         */
        glDisable(GL_BLEND);
        use();
        clear();

        gpass_shader->use();
        scene_manager->render(gpass_shader);

    }

    void light_pass(Shader *lightpass_shader, SceneManager *scene_manager, Quad q) {
        /*
         * Performs exclusively the light pass, must be used after the geometry pass.
         */
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);

        lightpass_shader->use();
        glActiveTexture(GL_TEXTURE0);
        use_texture(0);
        glActiveTexture(GL_TEXTURE1);
        use_texture(1);
        glActiveTexture(GL_TEXTURE2);
        use_texture(2);

        scene_manager->render(lightpass_shader);
        q.draw();

        lightpass_shader->dir_light_count = 0;
        lightpass_shader->point_light_count = 0;
    }

    void blit() {
        /*
         * Blits the GBuffer to the screen buffer. Should be used after the geometry and light pass.
         */
        glBindFramebuffer(GL_READ_FRAMEBUFFER, ID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);  // default
        glDepthMask(GL_TRUE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void draw(Shader *gpass_shader, Shader *lpass_shader, SceneManager *scene_manager, Quad *q) {
        /*
         * Performs the geometry and light pass then blits to the screen.
         */
        geometry_pass(gpass_shader, scene_manager);
        light_pass(lpass_shader, scene_manager, *q);
        blit();


    }



};


#endif //SDL3_FIRST_GBUFFER_H