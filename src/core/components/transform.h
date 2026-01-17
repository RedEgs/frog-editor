//
// Created by Charlie on 13/01/2026.
//

#ifndef SDL3_FIRST_TRANSFORM_H
#define SDL3_FIRST_TRANSFORM_H
#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "../component.h"


class Transform : public Component {
private:
    glm::mat4 matrix = glm::mat4(1.0f);
public:
    glm::vec3 position;
    glm::vec3 scale;

    Transform(glm::vec3 position, glm::vec3 scale) {
        this->position = position;
        this->scale = scale;
    }
    Transform() {
        position = glm::vec3(0);
        scale = glm::vec3(1);
    }

    glm::mat4 get_matrix() const {
        glm::mat4 m = glm::translate(matrix, position);
        m = glm::scale(m, scale);
        return m;
    }

    void properties_editor_imgui(Scene *scene) override {
        ImGui::InputFloat3("Position", value_ptr(position), "%.2f");
        ImGui::InputFloat3("Scale", value_ptr(scale), "%.2f");

        static ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;
        static ImGuizmo::MODE currentMode = ImGuizmo::WORLD;

        glm::mat4 model_matrix = get_matrix();
        glm::mat4 delta_matrix = matrix;
        ImGuizmo::Manipulate(
            glm::value_ptr(scene->main_camera.view_matrix),
            glm::value_ptr(scene->main_camera.project_matrix),
            currentOperation, currentMode, glm::value_ptr(model_matrix),
            glm::value_ptr(delta_matrix)
        );
        glm::vec3 deltaPos = glm::vec3(delta_matrix[3]);
        position += deltaPos;
    }

    static const char* get_static_class_name() { return "Transform"; };
    const char* get_class_name() override {return "TransformComponent";}

};
REGISTER_COMPONENT(Transform);


#endif //SDL3_FIRST_TRANSFORM_H
