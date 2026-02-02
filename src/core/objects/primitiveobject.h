//
// Created by Charlie on 17/01/2026.
//

#ifndef SDL3_FIRST_PRIMITIVEOBJECT_H
#define SDL3_FIRST_PRIMITIVEOBJECT_H
#include <glm/vec3.hpp>

#include "../gameobject.h"
#include "../components/primitive.h"

class PrimitiveObject : public GameObject {
public:
    PrimitiveObject(PrimitiveType prim_type, glm::vec3 position, glm::vec3 scale) {
        instance_name = "Primitive Object";
        this->add_component<Transform>(position, scale);
        this->add_component<Primitive>(prim_type);

    }

    PrimitiveObject() {
        instance_name = "Primitive Object";
        this->add_component<Transform>(glm::vec3(0.0), glm::vec3(1.0));
        this->add_component<Primitive>(CUBE);
    };


    const char * get_type_name() override {
        return "PrimitiveObject";
    };
};

REGISTER_GAME_OBJECT(PrimitiveObject);




#endif //SDL3_FIRST_PRIMITIVEOBJECT_H