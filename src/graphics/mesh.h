//
// Created by Charlie on 08/01/2026.
//

#ifndef SDL3_FIRST_MESH_H
#define SDL3_FIRST_MESH_H
#include "shader.h"
#include "texture.h"
#include "vertex.h"


class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        setup_mesh();
    }

    Mesh() {setup_mesh();}


    void draw(Shader &shader) {
        unsigned int diffuse_count = 1;
        unsigned int specular_count = 1;

        for (unsigned int i = 0; i < textures.size(); i++) {
            if (textures[i].ID == 0) continue; // skip invalid textures

            std::string number;
            if (textures[i].type == "diffuse") number = std::to_string(diffuse_count++);
            else if (textures[i].type == "specular") number = std::to_string(specular_count++);

            shader.setInt(("material." + textures[i].type + number).c_str(), i);
            glActiveTexture(GL_TEXTURE0);
            textures[i].use();

            //glBindTexture(GL_TEXTURE_2D, textures[i].ID);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    unsigned int VAO, VBO, EBO;

    void setup_mesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

        glBindVertexArray(0);
    }
};


#endif //SDL3_FIRST_MESH_H