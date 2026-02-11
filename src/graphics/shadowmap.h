//
// Created by User on 07/02/2026.
//

#ifndef SDL3_FIRST_SHADOWMAP_H
#define SDL3_FIRST_SHADOWMAP_H
#include <cstddef>
#include <glad/glad.h>

class Shadowmap {
private:

public:
    unsigned int FBO, depthMap;
    unsigned int resolution;
    glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
    Texture d;

    Shadowmap(int resolution = 1024) {
        this->resolution = resolution;

        glGenFramebuffers(1, &FBO);

        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void first_pass(unsigned int width, unsigned int height, Shader *depth_shader, SceneManager *scene_manager, glm::vec3 light_pos) {
        glCullFace(GL_FRONT);
        depth_shader->use();

        glm::mat4 lightView = glm::lookAt(light_pos,
                                  glm::vec3( 0.0f, 0.0f,  0.0f),
                                  glm::vec3( 0.0f, 1.0f,  0.0f));
        glm::mat4 lightSpaceMatrix = light_projection * lightView;
        depth_shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, resolution, resolution);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        scene_manager->render(depth_shader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glCullFace(GL_BACK);
        glViewport(0, 0, width, height);
    }
    //
    // void second_pass(Shader *shadow_renderer, SceneManager *scene_manager, glm::vec3 light_pos, GBuffer *gbuffer) {
    //     glBindFramebuffer(GL_FRAMEBUFFER, gbuffer->ID);
    //
    //     glActiveTexture(GL_TEXTURE3);
    //     glBindTexture(GL_TEXTURE_2D, gbuffer->textures[3].ID);
    //
    //     shadow_renderer->use();
    //     shadow_renderer->setVec3("lightPos", light_pos);
    //
    //     glm::mat4 lightView = glm::lookAt(light_pos,
    //                               glm::vec3( 0.0f, 0.0f,  0.0f),
    //                               glm::vec3( 0.0f, 1.0f,  0.0f));
    //     glm::mat4 lightSpaceMatrix = light_projection * lightView;
    //     shadow_renderer->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    //
    //     glActiveTexture(GL_TEXTURE0);
    //     glBindTexture(GL_TEXTURE_2D, d.ID);
    //     shadow_renderer->setInt("diffuseTexture", 0);
    //
    //     glActiveTexture(GL_TEXTURE1);
    //     glBindTexture(GL_TEXTURE_2D, depthMap);
    //     shadow_renderer->setInt("shadowMap", 1);
    //     scene_manager->render(shadow_renderer);
    //
    //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // }


};






#endif //SDL3_FIRST_SHADOWMAP_H