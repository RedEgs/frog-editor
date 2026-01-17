//
// Created by Charlie on 13/01/2026.
//

#ifndef SDL3_FIRST_MODELRENDERER_H
#define SDL3_FIRST_MODELRENDERER_H
#include "transform.h"
#include "../component.h"
#include "../../graphics/model.h"

class Model;

class ModelRenderer : public Component {
public:
    std::shared_ptr<Model> model;

    ModelRenderer(std::shared_ptr<Model> m) : model(std::move(m)) {
    }
    ModelRenderer() {
        std::string filename = choose_file_from_dialog();
        std::shared_ptr<Model> m(new Model(filename.c_str()));
        this->model = m;
    }

    static void SDLCALL file_callback(std::string *string, const char* const* filelist, int filter) {
        if (!filelist) {
            SDL_Log("An error occured: %s", SDL_GetError());
            return;
        } else if (!*filelist) {
            SDL_Log("The user did not select any file.");
            SDL_Log("Most likely, the dialog was canceled.");
            return;
        }

        while (*filelist) {
            SDL_Log("Full path to selected file: '%s'", *filelist);
            std::cout << " File chosen|!";
            string->append(*filelist);
            return;
        }

        if (filter < 0) {
            SDL_Log("The current platform does not support fetching "
                    "the selected filter, or the user did not select"
                    " any filter.");
            return;
        }


    };

    std::string choose_file_from_dialog() {
        std::string path = "";
        SDL_DialogFileFilter filter {
            .name = "OBJ Files",
            .pattern = "obj",
        };
        SDL_ShowOpenFileDialog(SDL_DialogFileCallback(ModelRenderer::file_callback), &path, NULL, &filter, 1, NULL, false);
        while (path == "") {
            //std::cout << "CHOOSING!!" << std::endl;
        }
        return path;
    }

    void on_render(Shader *shader) override {
        auto* t = owner->get_component<Transform>();
        if (t == NULL) {
            shader->setMat4("model", glm::mat4(1.0));
        } else {
            shader->setMat4("model", t->get_matrix());
        }

        model->draw(*shader);
    }

    void properties_editor_imgui(Scene *scene) override {
        std::string x = "Model Path: ";
        x.append(model.get()->path);
        ImGui::Text(x.c_str());
    };

    static const char* get_static_class_name() { return "Model Renderer"; };
    const char* get_class_name() override {return "ModelRendererComponent";}

};
REGISTER_COMPONENT(ModelRenderer);



#endif //SDL3_FIRST_MODELRENDERER_H