//
// Created by User on 04/02/2026.
//

#ifndef SDL3_FIRST_BILLBOARD_H
#define SDL3_FIRST_BILLBOARD_H
#include <glm/vec3.hpp>

#include "texture.h"


class Billboard {
private:
    static constexpr float billboard_vertices[20] = {
        .5, .5, 1, 0, 0, // Vertex 0: pos(0,0,1), uv(0,0)
        -.5, .5, 1, 1, 0, // Vertex 1: pos(1,0,1), uv(1,0)
        .5, -.5, 1, 0, 1, // Vertex 2: pos(0,1,1), uv(0,1)
        -.5, -.5, 1, 1, 1 // Vertex 3: pos(1,1,1), uv(1,1)
    };

    // Indices for triangle strip alternative
    static constexpr unsigned int billboard_indices[6] = {
        0, 1, 2, // First triangle
        1, 3, 2 // Second triangle
    };

    unsigned int VBO, VAO, EBO; // Need EBO for indexed drawing
public:
    Texture texture;
    glm::vec3 position = glm::vec3(0.0f);
    float size = 1.0f;
    bool ylock = false;

    Billboard(const Texture &texture) {
        this->texture = texture;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO); // Generate EBO

        glBindVertexArray(VAO);

        // 1. Setup vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(billboard_vertices),
            billboard_vertices,
            GL_STATIC_DRAW
        );

        // 2. Setup index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof(billboard_indices),
            billboard_indices,
            GL_STATIC_DRAW
        );

        // 3. Setup vertex attributes
        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0, // attribute index 0
            3, // 3 components (x, y, z)
            GL_FLOAT, // type
            GL_FALSE, // normalized
            5 * sizeof(float), // stride: 5 floats per vertex (3 pos + 2 uv)
            (void *) 0 // offset: position starts at 0
        );

        // Texture coordinate attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1, // attribute index 1
            2, // 2 components (u, v)
            GL_FLOAT, // type
            GL_FALSE, // normalized
            5 * sizeof(float), // stride: same as above
            (void *) (3 * sizeof(float)) // offset: uv starts after 3 position floats
        );

        glBindVertexArray(0);
    }

    void draw(Shader *shader) {
        glEnable(GL_BLEND);

        glm::mat4 model_mat = glm::mat4(1.0);
        model_mat = glm::translate(model_mat, position);
        model_mat = glm::scale(model_mat, glm::vec3(.5f));

        shader->setMat4("model", model_mat);
        shader->setBool("ylock", ylock);
        shader->setFloat("size", size);

        glActiveTexture(GL_TEXTURE0);
        texture.use();

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glDisable(GL_BLEND);
    }

    ~Billboard() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
};


#endif //SDL3_FIRST_BILLBOARD_H
