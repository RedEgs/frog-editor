//
// Created by User on 04/02/2026.
//

#ifndef SDL3_FIRST_QUAD_H
#define SDL3_FIRST_QUAD_H
#include <glad/glad.h>

class Shader;

class Quad {
private:
    static constexpr float quad_vertices[] = {
        -1.f, -1.f, 0.f, 0.f, 0.f,
         1.f, -1.f, 0.f, 1.f, 0.f,
         1.f,  1.f, 0.f, 1.f, 1.f,
        -1.f,  1.f, 0.f, 0.f, 1.f
    };

    // Indices for triangle strip alternative
    static constexpr unsigned int quad_indices[6] = {
        0, 1, 2, // First triangle
        1, 3, 2 // Second triangle
    };

    unsigned int VBO, VAO, EBO; // Need EBO for indexed drawing
public:
    Quad() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO); // Generate EBO

        glBindVertexArray(VAO);

        // 1. Setup vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(quad_vertices),
            quad_vertices,
            GL_STATIC_DRAW
        );

        // 2. Setup index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof(quad_vertices),
            quad_indices,
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

    void draw() {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    ~Quad() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
};

#endif //SDL3_FIRST_QUAD_H