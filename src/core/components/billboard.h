//
// Created by User on 03/02/2026.
//

#include "graphics/billboard.h"
#include "graphics/texture.h"


#ifndef SDL3_FIRST_BILLBOARDCOMPONENT_H
#define SDL3_FIRST_BILLBOARDCOMPONENT_H



class BillboardComponent: public Component {
public:
    glm::vec3 position = glm::vec3(0.0f);
    Texture texture = Texture();
    Billboard billboard = Billboard(texture);


    BillboardComponent (Texture texture) : billboard(texture) {
        this->texture = texture;
        billboard.texture = texture;
    }

    BillboardComponent(){}


    void properties_editor_imgui(Scene *scene) override {
        auto* t = owner->get_component<Transform>();

        if (t != NULL) {
            static bool shouldRenderGizmo = true;
            ImGui::Checkbox("Render Gizmo", &shouldRenderGizmo);
            if (shouldRenderGizmo) {

                static ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;
                static ImGuizmo::MODE currentMode = ImGuizmo::WORLD;

                glm::mat4 m = glm::mat4(1.0f);
                glm::mat4 dp = glm::mat4(1.0f);
                m = glm::translate(m, t->position);

                glm::mat4 model_matrix = m;
                glm::mat4 delta_matrix = dp;
                ImGuizmo::Manipulate(
                    glm::value_ptr(scene->main_camera.view_matrix),
                    glm::value_ptr(scene->main_camera.project_matrix),
                    currentOperation, currentMode, glm::value_ptr(model_matrix),
                    glm::value_ptr(delta_matrix)
                );
                glm::vec3 deltaPos = glm::vec3(delta_matrix[3]);
                t->position+=deltaPos;
            }
            ImGui::InputFloat3("Position", glm::value_ptr(t->position), "%.2f");
        } else {
            ImGui::InputFloat3("Position", glm::value_ptr(position), "%.2f");
        }

        ImGui::Checkbox("Y-Locked", &billboard.ylock);

    };

    void on_render(Shader *shader, Camera camera, Renderer *renderer) override {
        if (shader->name == "billboard") {
            glDisable(GL_CULL_FACE);

            auto* t = owner->get_component<Transform>();

            billboard.draw(shader);
            if (t != NULL) {
                billboard.position =  t->position;
            } else {
                billboard.position = position;
            }

            glEnable(GL_CULL_FACE);
        }
    }

    static const char* get_static_class_name() { return "Billboard"; };
    const char* get_class_name() override { return "BillboardComponent"; }
};
REGISTER_COMPONENT(BillboardComponent);


#endif //SDL3_FIRST_BILLBOARDCOMPONENT_H