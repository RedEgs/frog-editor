//
// Created by Charlie on 13/01/2026.
//
#pragma once
#ifndef SDL3_FIRST_MODELRENDERER_H
#define SDL3_FIRST_MODELRENDERER_H

class ModelRenderer : public Component {
private:
    SDL_DialogFileFilter default_fdfilter {
        .name = "OBJ Files",
        .pattern = "obj",
    };

public:
    std::shared_ptr<Model> model;

    ModelRenderer(std::shared_ptr<Model> m) : model(std::move(m)) {
    }
    ModelRenderer() {
        std::string filename = Files::open_file_dialogue_blocking(default_fdfilter);
        std::shared_ptr<Model> m(new Model(filename.c_str()));
        this->model = m;
    }

    void on_render(Shader *shader, Camera camera, Renderer *renderer) override {
        if (shader->name != "forward-renderer" && shader->name != "geometry-pass" && shader->name != "shadow-pass" && shader->name != "shadow-renderer")
            return;

        auto* t = owner->get_component<Transform>();
        if (t == nullptr) {
            shader->setMat4("model", glm::mat4(1.0));
        } else {
            shader->setMat4("model", t->get_matrix());
        }

        model->draw(*shader);
    }

    void properties_editor_imgui(Scene *scene) override {
        std::string x = "Model Path: ";
        x.append(model->path);
        ImGui::Text(x.c_str());
    };

    static const char* get_static_class_name() { return "Model Renderer"; };
    const char* get_class_name() override {return "ModelRendererComponent";}

};
REGISTER_COMPONENT(ModelRenderer);



#endif //SDL3_FIRST_MODELRENDERER_H