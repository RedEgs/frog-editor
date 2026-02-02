//
// Created by Charlie on 08/01/2026.
//

#ifndef SDL3_FIRST_MODEL_H
#define SDL3_FIRST_MODEL_H
#include "mesh.h"
#include "shader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class Model {
public:
    std::string path;

    explicit Model(const char *filepath) {
        path = static_cast<std::string>(filepath);
        load_model(filepath);
    }

    void draw(Shader &shader) {
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].draw(shader);
    }

private:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;

    void load_model(const char * path) {
        Assimp::Importer import;

        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_GenUVCoords);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ASSIMP Error: %s", import.GetErrorString());
            return;
        }
        std::string path_str(path);
        directory = path_str.substr(0, path_str.find_last_of('/'));

        process_node(scene->mRootNode, scene);
    }

    void process_node(aiNode *node, const aiScene *scene) {
        // process all the node's meshes (if any)
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(process_mesh(mesh, scene));
        }
        // then do the same for each of its children
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            process_node(node->mChildren[i], scene);
        }
    }

    Mesh process_mesh(aiMesh *mesh, const aiScene *scene) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            aiVector3D v_pos = mesh->mVertices[i];
            aiVector3D v_norm = mesh->mNormals[i];

            glm::vec2 tex_coords(0.0f, 0.0f);
            if (mesh->mTextureCoords[0]) { // check first UV channel
                tex_coords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }

            auto vertex = Vertex{
                .position = glm::vec3(v_pos.x, v_pos.y, v_pos.z),
                .normal = glm::vec3(v_norm.x, v_norm.y, v_norm.z),
                .tex_coords = tex_coords
            };
            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        if(mesh->mMaterialIndex >= 0)
        {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            std::vector<Texture> diffuseMaps = load_material_textures(material,
                                                                    aiTextureType_DIFFUSE, "diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            std::vector<Texture> specularMaps = load_material_textures(material,
                                                                     aiTextureType_SPECULAR, "specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }

        return Mesh(vertices, indices, textures);

    }

    std::vector<Texture> load_material_textures(aiMaterial *mat, aiTextureType type, std::string typeName)
    {
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            bool skip = false;
            for (auto& loaded : textures_loaded) {
                if (str.C_Str() == loaded.path) { // already loaded
                    textures.push_back(loaded);
                    skip = true;
                    break;
                }
            }

            if (!skip) {
                std::string pathstr("models/");
                pathstr.append(str.C_Str());

                Texture tex(pathstr.c_str(), typeName);
                if (tex.ID != 0) {                // only push valid textures
                    textures.push_back(tex);
                    textures_loaded.push_back(tex); // store globally to skip duplicates
                }
            }
        }
        return textures;
    }
};


#endif //SDL3_FIRST_MODEL_H