//
// Created by User on 02/02/2026.
//

#ifndef SDL3_FIRST_SKY_H
#define SDL3_FIRST_SKY_H
#include <SDL3/SDL_dialog.h>

#include "../component.h"
#include "../../utility/files.h"
#include "../../graphics/cubemap.h"


struct File;

class Sky: public Component {
private:
    SDL_DialogFileFilter default_fdfilter {
        .name = "Zip Files",
        .pattern = "zip",
    };

    std::unique_ptr<Cubemap> cubemap;

public:
    Sky() {
        std::string filename = Files::open_file_dialogue_blocking(default_fdfilter);
        std::vector<File> uzip = Files::unzip(Files::load_file_to_memory(filename));
        std::vector<std::vector<uint8_t>> rdata(6);

        constexpr std::array<std::pair<std::string_view, size_t>, 6> faces = {{
            {"right.jpg",  0},
            {"left.jpg",   1},
            {"top.jpg",    2},
            {"bottom.jpg", 3},
            {"front.jpg",  4},
            {"back.jpg",   5},
        }};
        for (const File& f : uzip) {
            for (auto [name, idx] : faces) {
                if (f.name == name) {
                    rdata[idx] = f.data;
                    break;
                }
            }
        }

        cubemap = std::make_unique<Cubemap>(rdata);

    };

    void properties_editor_imgui(Scene *scene) override {
        ImGui::Button("Hello!");
    };

    void on_render(Shader *shader, Camera camera, Renderer *renderer) override {
        if (shader->name == "skybox") {
            cubemap->draw(camera.view_matrix, shader);
        }
    }

    static const char* get_static_class_name() { return "Sky"; };
    const char* get_class_name() override { return "SkyComponent"; }
};
REGISTER_COMPONENT(Sky);
#endif //SDL3_FIRST_SKY_H
