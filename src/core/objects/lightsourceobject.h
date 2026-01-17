//
// Created by Charlie on 14/01/2026.
//

#ifndef SDL3_FIRST_LIGHTSOURCEOBJECT_H
#define SDL3_FIRST_LIGHTSOURCEOBJECT_H
#include "../gameobject.h"
#include "../components/lightsource.h"


class LightSourceObject : public GameObject {
public:
    LightSourceObject(std::unique_ptr<LightType> light_type) {
        instance_name = "Light Source Object";
        this->add_component<LightSource>(std::move(light_type));
    }

    const char * get_type_name() override {
        return "LightSourceObject";
    };
};


#endif //SDL3_FIRST_LIGHTSOURCEOBJECT_H