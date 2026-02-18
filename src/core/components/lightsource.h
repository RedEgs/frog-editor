//
// Created by Charlie on 13/01/2026.
//
#pragma once
#ifndef SDL3_FIRST_LIGHTSOURCE_H
#define SDL3_FIRST_LIGHTSOURCE_H

#include <imgui.h>

#include "transform.h"
#include "../../graphics/shader.h"
#include "../../graphics/billboard.h"

enum LightTypeEnum {
    DIRECTIONAL = 0,
    POINT = 1,
};

struct LightType {
    virtual ~LightType() = default;
};

struct DirectionalLightType : LightType {
    glm::vec3 direction{};

    glm::vec3 ambient{};
    glm::vec3 diffuse{};
    glm::vec3 specular{};

    float intensity;
    bool cast_shadow;

    DirectionalLightType(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float intensity, bool cast_shadow): direction(direction), ambient(ambient), diffuse(diffuse), specular(specular), intensity(intensity), cast_shadow(cast_shadow){}
    static std::unique_ptr<LightType> MakeSun() {
        return std::make_unique<DirectionalLightType>(
            glm::vec3(-0.2f, -1.0f, -0.3f),
            glm::vec3(1.0f),
            glm::vec3(0.4f),
            glm::vec3(0.5f),
            1.0,
            false
        );
    }
};

struct PointLightType : LightType {
    glm::vec3 position{};
    glm::vec3 ambient{};
    glm::vec3 diffuse{};
    glm::vec3 specular{};

    float intensity;
    float range;
    bool cast_shadow;

    PointLightType(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float intensity, float range, bool cast_shadow):
        position(position), ambient(ambient), diffuse(diffuse), specular(specular), intensity(intensity), range(range), cast_shadow(cast_shadow) {}

    static std::unique_ptr<LightType> Regular() {
        return std::make_unique<PointLightType>(
            glm::vec3(0, 0, 0),
            glm::vec3(1.0f),
            glm::vec3(0.4f),
            glm::vec3(0.5f),
            1.0,
            3.0,
            false
        );
    }
};

class LightSource : public Component {
private:
    const char* combo_types[2] = {"Directional Light", "Point Light"};
    int selected_combo_type = 0;

    Texture dir_icon = Texture("../resources/icons/directional-light.png", "diffuse");
    Texture point_icon = Texture("../resources/icons/point-light.png", "diffuse");

    Billboard icon = Billboard(point_icon);
    bool render_icon = true;
public:
    std::unique_ptr<LightType> light_type;

    LightSource(std::unique_ptr<LightType> lt): light_type(std::move(lt))
    {
    }

    LightSource()
    {
        light_type = DirectionalLightType::MakeSun();
        selected_combo_type = 0;
    }

    void properties_editor_imgui(Scene* scene) override {
        auto* t = owner->get_component<Transform>();




        bool x = ImGui::Combo("Light Type", &selected_combo_type, combo_types, IM_ARRAYSIZE(combo_types));
        if (x) {
            LightTypeEnum selected_type = LightTypeEnum(selected_combo_type);
            if (selected_type != get_light_type()) {
                if (get_light_type() == POINT) {
                    auto* p = dynamic_cast<PointLightType*>(light_type.get());

                    light_type = std::make_unique<DirectionalLightType>(
                        p->position,      // reusing position as direction
                        p->ambient,
                        p->diffuse,
                        p->specular,
                        p->intensity,
                        p->cast_shadow
                    );

                } else if (get_light_type() == DIRECTIONAL) {
                    auto* d = dynamic_cast<DirectionalLightType*>(light_type.get());

                    light_type = std::make_unique<PointLightType>(
                        d->direction,     // reusing direction as position
                        d->ambient,
                        d->diffuse,
                        d->specular,
                        d->intensity,
                        3.0f,
                        d->cast_shadow
                    );

                }


            }
        }

        ImGui::Separator();

        ImGui::Checkbox("Render Icon", &render_icon);

        if (auto* p = dynamic_cast<PointLightType*>(light_type.get())) {
            // point light UI

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

                ImGui::Separator();
                ImGui::InputFloat3("Position", glm::value_ptr(p->position), "%.2f");
            }

            ImGui::Separator();

            ImGui::ColorEdit3("Ambient", glm::value_ptr(p->ambient));
            ImGui::ColorEdit3("Diffuse", glm::value_ptr(p->diffuse));
            ImGui::ColorEdit3("Specular", glm::value_ptr(p->specular));

            ImGui::Separator();

            ImGui::SliderFloat("Intensity", &p->intensity, 0, 100);
            ImGui::SliderFloat("Range", &p->range, 0, 20);
            ImGui::Checkbox("Cast Shadow", &p->cast_shadow);


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

                ImGui::Separator();
                ImGui::InputFloat3("Direction", glm::value_ptr(d->direction), "%.2f");
            }

            ImGui::Separator();

            ImGui::ColorEdit3("Ambient", glm::value_ptr(d->ambient));
            ImGui::ColorEdit3("Diffuse", glm::value_ptr(d->diffuse));
            ImGui::ColorEdit3("Specular", glm::value_ptr(d->specular));

            ImGui::Separator();

            ImGui::SliderFloat("Intensity", &d->intensity, 0, 100);
            ImGui::Checkbox("Cast Shadow", &d->cast_shadow);
        }
    }

    void on_render(Shader *shader, Camera camera, Renderer *renderer) override {
        auto* t = owner->get_component<Transform>();

        if (render_icon) {
            if (shader->name == "billboard") {
                glm::vec3 position;
                icon.size = .5f;

                if (auto* p = dynamic_cast<PointLightType*>(light_type.get())) {
                    position = p->position;
                    icon.position = position;

                }
                else if (auto* d = dynamic_cast<DirectionalLightType*>(light_type.get())) {
                    position = d->direction;

                    glm::mat4 m = glm::mat4(1.0f);
                    m = glm::translate(m, d->direction);
                    m = inverse(m);

                    icon.position = glm::vec3(m[3]);
                }

                icon.draw(shader);
            }
        }

        if (shader->name != "light-pass" && shader->name != "forward-renderer" && shader->name != "shadow-pass" && shader->name != "shadow-renderer") return;
        if (auto* p = dynamic_cast<PointLightType*>(light_type.get())) {
            // point light UI
            if (t != NULL) {
                p->position = t->position;
            }
        }
        else if (auto* d = dynamic_cast<DirectionalLightType*>(light_type.get())) {
            if (t != NULL) {
                glm::mat4 m = inverse(t->get_matrix());
                glm::vec3 dir = glm::vec3(m[3]);

                d->direction = dir;
            }
        }


    }

    glm::vec3 get_position() {
        auto* t = owner->get_component<Transform>();

        if (t != NULL) {
            return t->position;
        }

        if (auto* p = dynamic_cast<PointLightType*>(light_type.get())) {
             return p->position;
        } else if (auto* d = dynamic_cast<DirectionalLightType*>(light_type.get())) {
            return d->direction;
        }
    }

    LightTypeEnum get_light_type() {
        if (auto* p = dynamic_cast<PointLightType*>(light_type.get())) {
            return POINT;
        }
        if (auto* d = dynamic_cast<DirectionalLightType*>(light_type.get())) {
            return DIRECTIONAL;
        }
    }

    std::string get_light_type_name() {
        LightTypeEnum x = get_light_type();

        switch (x) {
            case POINT:
                return "Point Light"; break;
            case DIRECTIONAL:
                return "Directional Light"; break;
        }

        return "None";
    }

    static const char* get_static_class_name() { return "Light Source"; };
    const char* get_class_name() override { return "LightSourceComponent"; }

};
REGISTER_COMPONENT(LightSource);


#endif //SDL3_FIRST_LIGHTSOURCE_H