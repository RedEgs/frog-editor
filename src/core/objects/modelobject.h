//
// Created by Charlie on 13/01/2026.
//

#ifndef SDL3_FIRST_MODELOBJECT_H
#define SDL3_FIRST_MODELOBJECT_H
#include <glm/vec3.hpp>

#include "../gameobject.h"
#include "../../graphics/model.h"
#include "../components/modelrenderer.h"

class ModelObject : public GameObject {
public:
    ModelObject(std::shared_ptr<Model> model, glm::vec3 position, glm::vec3 scale) {
        instance_name = "Model Object";
        this->add_component<Transform>(position, scale);
        this->add_component<ModelRenderer>(model);
    }

    ModelObject() {
        instance_name = "Model Object";
        this->add_component<Transform>(glm::vec3(0), glm::vec3(1));
        this->add_component<ModelRenderer>();
    }

    const char * get_type_name() override {
        return "ModelObject";
    };
};

REGISTER_GAME_OBJECT(ModelObject);


#endif //SDL3_FIRST_MODELOBJECT_H