//
// Created by Charlie on 13/01/2026.
//

#ifndef SDL3_FIRST_LIGHTSOURCE_H
#define SDL3_FIRST_LIGHTSOURCE_H
#include <imgui.h>
#include <glm/vec3.hpp>

#include "transform.h"
#include "../component.h"
#include "../../graphics/shader.h"
#include "../../graphics/billboard.h"

struct LightType {
    virtual ~LightType() = default;
};

struct DirectionalLightType : LightType {
    glm::vec3 direction{};
    glm::vec3 ambient{};
    glm::vec3 diffuse{};
    glm::vec3 specular{};

    DirectionalLightType(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular): direction(direction), ambient(ambient), diffuse(diffuse), specular(specular) {}
    static std::unique_ptr<LightType> MakeSun() {
        return std::make_unique<DirectionalLightType>(
            glm::vec3(-0.2f, -1.0f, -0.3f),
            glm::vec3(1.0f),
            glm::vec3(0.4f),
            glm::vec3(0.5f)
        );
    }
};

struct PointLightType : LightType {
    float intensity;
    glm::vec3 position{};
    glm::vec3 ambient{};
    glm::vec3 diffuse{};
    glm::vec3 specular{};

    PointLightType(float intensity, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular):
        intensity(intensity), position(position), ambient(ambient), diffuse(diffuse), specular(specular) {}
    static std::unique_ptr<LightType> Regular() {
        return std::make_unique<PointLightType>(
            1.0,
            glm::vec3(0, 0, 0),
            glm::vec3(1.0f),
            glm::vec3(0.4f),
            glm::vec3(0.5f)
        );
    }
};

class LightSource : public Component {
private:
    Texture icon_tex = Texture("../resources/icons/light-source.png", "diffuse");
    Billboard icon = Billboard(icon_tex);
    bool render_icon = true;
public:
    std::unique_ptr<LightType> light_type;

    LightSource(std::unique_ptr<LightType> lt) : light_type(std::move(lt)) {}
    LightSource() {
        light_type = DirectionalLightType::MakeSun();
    }

    void properties_editor_imgui(Scene* scene) override {
        auto* t = owner->get_component<Transform>();

        ImGui::Checkbox("Render Icon", &render_icon);

        if (auto* p = dynamic_cast<PointLightType*>(light_type.get())) {
            // point light UI
            ImGui::SliderFloat("Intensity", &p->intensity, 0, 5);
            if (t == NULL) {
                static bool shouldRenderGizmo = true;
                ImGui::Checkbox("Render Gizmo", &shouldRenderGizmo);
                if (shouldRenderGizmo) {

                    static ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;
                    static ImGuizmo::MODE currentMode = ImGuizmo::WORLD;

                    glm::mat4 m = glm::mat4(1.0f);
                    glm::mat4 dp = glm::mat4(1.0f);
                    m = glm::translate(m, p->position);

                    glm::mat4 model_matrix = m;
                    glm::mat4 delta_matrix = dp;
                    ImGuizmo::Manipulate(
                        glm::value_ptr(scene->main_camera.view_matrix),
                        glm::value_ptr(scene->main_camera.project_matrix),
                        currentOperation, currentMode, glm::value_ptr(model_matrix),
                        glm::value_ptr(delta_matrix)
                    );
                    glm::vec3 deltaPos = glm::vec3(delta_matrix[3]);
                    p->position+=deltaPos;



                }

                ImGui::InputFloat3("Position", glm::value_ptr(p->position), "%.2f");
            }

            ImGui::ColorEdit3("Ambient", glm::value_ptr(p->ambient));
            ImGui::ColorEdit3("Diffuse", glm::value_ptr(p->diffuse));
            ImGui::ColorEdit3("Specular", glm::value_ptr(p->specular));
        }
        else if (auto* d = dynamic_cast<DirectionalLightType*>(light_type.get())) {
            // directional light UI
            if (t == NULL) {
                static bool shouldRenderGizmo = true;
                ImGui::Checkbox("Render Gizmo", &shouldRenderGizmo);
                if (shouldRenderGizmo) {

                    static ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;
                    static ImGuizmo::MODE currentMode = ImGuizmo::WORLD;

                    glm::mat4 m = glm::mat4(1.0f);
                    glm::mat4 dp = glm::mat4(1.0f);
                    m = glm::translate(m, d->direction);
                    m = inverse(m);
                    dp = inverse(dp);

                    glm::mat4 model_matrix = m;
                    glm::mat4 delta_matrix = dp;
                    ImGuizmo::Manipulate(
                        glm::value_ptr(scene->main_camera.view_matrix),
                        glm::value_ptr(scene->main_camera.project_matrix),
                        currentOperation, currentMode, glm::value_ptr(model_matrix),
                        glm::value_ptr(delta_matrix)
                    );
                    delta_matrix = glm::inverse(delta_matrix);

                    glm::vec3 deltaPos = glm::vec3(delta_matrix[3]);
                    d->direction+=deltaPos;

                }

                ImGui::InputFloat3("Direction", glm::value_ptr(d->direction), "%.2f");
            }

            ImGui::ColorEdit3("Ambient", glm::value_ptr(d->ambient));
            ImGui::ColorEdit3("Diffuse", glm::value_ptr(d->diffuse));
            ImGui::ColorEdit3("Specular", glm::value_ptr(d->specular));
        }
    }

    void on_render(Shader *shader, Camera camera) override {
        auto* t = owner->get_component<Transform>();

        if (render_icon) {
            if (shader->name == "billboard") {
                glm::vec3 position;
                if (auto* p = dynamic_cast<PointLightType*>(light_type.get())) {
                    position = p->position;
                }
                else if (auto* d = dynamic_cast<DirectionalLightType*>(light_type.get())) {
                    position = d->direction;
                }
                icon.size = .5f;
                icon.position = position;
                icon.draw(shader);
            }
        }

        if (shader->name != "light-pass" && shader->name != "forward-renderer") return;
        if (auto* p = dynamic_cast<PointLightType*>(light_type.get())) {
            // point light UI
            if (t != NULL) {
                p->position = t->position;
            }

            int i = shader->point_light_count++;
            std::string base = "point_lights[" + std::to_string(i) + "]";

            shader->setFloat(base + ".intensity", p->intensity);

            shader->setVec3(base + ".position", p->position);
            shader->setVec3(base + ".ambient", p->ambient);
            shader->setVec3(base + ".diffuse", p->diffuse);
            shader->setVec3(base + ".specular", p->specular);

            shader->setBool(base + ".enabled", true);
        }
        else if (auto* d = dynamic_cast<DirectionalLightType*>(light_type.get())) {
            if (t != NULL) {
                glm::mat4 m = inverse(t->get_matrix());
                glm::vec3 dir = glm::vec3(m[3]);

                d->direction = dir;
            }

            // directional light UI
            int i = shader->dir_light_count++;
            std::string base = "directional_lights[" + std::to_string(i) + "]";

            shader->setVec3(base + ".direction", d->direction);
            shader->setVec3(base + ".ambient", d->ambient);
            shader->setVec3(base + ".diffuse", d->diffuse);
            shader->setVec3(base + ".specular", d->specular);

            shader->setBool(base + ".enabled", true);
        }
    }

    static const char* get_static_class_name() { return "Light Source"; };
    const char* get_class_name() override { return "LightSourceComponent"; }

};
REGISTER_COMPONENT(LightSource);


#endif //SDL3_FIRST_LIGHTSOURCE_H