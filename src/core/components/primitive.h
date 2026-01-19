//
// Created by Charlie on 17/01/2026.
//

#ifndef SDL3_FIRST_PRIMITIVE_H
#define SDL3_FIRST_PRIMITIVE_H


#include <imgui.h>

#include "transform.h"
#include "../component.h"
#include "../../graphics/mesh.h"


struct Vertex;
class Texture;

enum PrimitiveType {
    CUBE,
    SPHERE,
    PLANE
};

inline const char *primitive_type_to_string(PrimitiveType pt) {
    switch (pt) {
        case CUBE:
            return "Cube";
        case SPHERE:
            return "Sphere";
        case PLANE:
            return "Plane";
    }
    return "None";
}

Mesh inline CreateCube(float size = 1.0f) {
    float h = size * 0.5f;
    std::vector<Texture> texture;
    std::vector<Vertex> vertices = {
        // +Z (front)
        {{-h, -h,  h}, {0, 0, 1}, {0, 0}},
        {{ h, -h,  h}, {0, 0, 1}, {1, 0}},
        {{ h,  h,  h}, {0, 0, 1}, {1, 1}},
        {{-h,  h,  h}, {0, 0, 1}, {0, 1}},

        // -Z (back)
        {{ h, -h, -h}, {0, 0, -1}, {0, 0}},
        {{-h, -h, -h}, {0, 0, -1}, {1, 0}},
        {{-h,  h, -h}, {0, 0, -1}, {1, 1}},
        {{ h,  h, -h}, {0, 0, -1}, {0, 1}},

        // +X (right)
        {{ h, -h,  h}, {1, 0, 0}, {0, 0}},
        {{ h, -h, -h}, {1, 0, 0}, {1, 0}},
        {{ h,  h, -h}, {1, 0, 0}, {1, 1}},
        {{ h,  h,  h}, {1, 0, 0}, {0, 1}},

        // -X (left)
        {{-h, -h, -h}, {-1, 0, 0}, {0, 0}},
        {{-h, -h,  h}, {-1, 0, 0}, {1, 0}},
        {{-h,  h,  h}, {-1, 0, 0}, {1, 1}},
        {{-h,  h, -h}, {-1, 0, 0}, {0, 1}},

        // +Y (top)
        {{-h,  h,  h}, {0, 1, 0}, {0, 0}},
        {{ h,  h,  h}, {0, 1, 0}, {1, 0}},
        {{ h,  h, -h}, {0, 1, 0}, {1, 1}},
        {{-h,  h, -h}, {0, 1, 0}, {0, 1}},

        // -Y (bottom)
        {{-h, -h, -h}, {0, -1, 0}, {0, 0}},
        {{ h, -h, -h}, {0, -1, 0}, {1, 0}},
        {{ h, -h,  h}, {0, -1, 0}, {1, 1}},
        {{-h, -h,  h}, {0, -1, 0}, {0, 1}},
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,  2, 3, 0,        // front
        4, 5, 6,  6, 7, 4,        // back
        8, 9,10, 10,11, 8,        // right
       12,13,14, 14,15,12,        // left
       16,17,18, 18,19,16,        // top
       20,21,22, 22,23,20         // bottom
    };

    Mesh m(vertices, indices, texture);
    return m;
}
Mesh inline CreateSphere(float radius = 0.5f, int rings = 16, int sectors = 32) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> texture;

    const float PI = 3.14159265359f;

    for (int r = 0; r <= rings; ++r) {
        float v = float(r) / float(rings);
        float phi = v * PI; // 0..PI

        for (int s = 0; s <= sectors; ++s) {
            float u = float(s) / float(sectors);
            float theta = u * 2.0f * PI; // 0..2PI

            float x = std::sin(phi) * std::cos(theta);
            float y = std::cos(phi);
            float z = std::sin(phi) * std::sin(theta);

            Vertex vert;
            vert.position   = glm::vec3(x, y, z) * radius;
            vert.normal     = glm::normalize(glm::vec3(x, y, z));
            vert.tex_coords = glm::vec2(u, 1.0f - v);

            vertices.push_back(vert);
        }
    }

    int stride = sectors + 1;

    for (int r = 0; r < rings; ++r) {
        for (int s = 0; s < sectors; ++s) {
            uint32_t i0 = r * stride + s;
            uint32_t i1 = i0 + stride;
            uint32_t i2 = i1 + 1;
            uint32_t i3 = i0 + 1;

            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i2);

            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i3);
        }
    }

    Mesh m(vertices, indices, texture);
    return m;
}
Mesh inline CreatePlane(float size = 1.0f, int subdivisions = 1) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> texture;


    int vertsPerSide = subdivisions + 1;
    float half = size * 0.5f;
    float step = size / subdivisions;

    for (int z = 0; z < vertsPerSide; ++z) {
        for (int x = 0; x < vertsPerSide; ++x) {

            float px = -half + x * step;
            float pz = -half + z * step;

            Vertex v;
            v.position   = { px, 0.0f, pz };
            v.normal     = { 0.0f, 1.0f, 0.0f };
            v.tex_coords = {
                float(x) / subdivisions,
                float(z) / subdivisions
            };

            vertices.push_back(v);
        }
    }

    for (int z = 0; z < subdivisions; ++z) {
        for (int x = 0; x < subdivisions; ++x) {

            int i0 =  z      * vertsPerSide + x;
            int i1 = (z + 1) * vertsPerSide + x;
            int i2 = (z + 1) * vertsPerSide + x + 1;
            int i3 =  z      * vertsPerSide + x + 1;

            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i2);

            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i3);
        }
    }

    Mesh m(vertices, indices, texture);
    return m;
}



class Primitive : public Component {
private:
    PrimitiveType primitive_type;
    Mesh mesh;
public:
    Primitive(PrimitiveType primitive) {
        primitive_type = primitive;
        construct_mesh_from_type();
    }

    Primitive() {
        primitive_type = CUBE;
        mesh = CreateCube();
    }

    void construct_mesh_from_type() {
        switch (primitive_type) {
            case CUBE:
                mesh = CreateCube();
                break;
            case SPHERE:
                mesh = CreateSphere();
                break;
            case PLANE:
                mesh = CreatePlane();
                break;
            default:
                mesh = CreateCube();
                break;
        }
    }

    void properties_editor_imgui(Scene *scene) override {
        static std::vector<const char *> prim_names;
        static int current_type = primitive_type;

        if (prim_names.size() < PLANE+1) {
            for (int i = 0; i < PLANE+1; i++) {
                PrimitiveType p = static_cast<PrimitiveType>(i);
                prim_names.push_back(primitive_type_to_string(p));
            }
        }

        if (ImGui::Combo("Primitive Type", &current_type, prim_names.data(), prim_names.size())) {
            primitive_type = static_cast<PrimitiveType>(current_type);
            construct_mesh_from_type();
        }
    }

    void on_render(Shader *shader) override {
        auto* t = owner->get_component<Transform>();
        if (t == nullptr) {
            shader->setMat4("model", glm::mat4(1.0));
        } else {
            shader->setMat4("model", t->get_matrix());
        }

        mesh.draw(*shader);
    }

    Mesh get_mesh() {
        return this->mesh;
    }

    PrimitiveType get_primitive_type() const {
        return this->primitive_type;
    }

    static const char* get_static_class_name() { return "Primitive"; };
    const char* get_class_name() override {return "PrimitiveComponent";}

};
REGISTER_COMPONENT(Primitive);

#endif //SDL3_FIRST_PRIMITIVE_H