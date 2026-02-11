//
// Created by Charlie on 13/01/2026.
//

#ifndef SDL3_FIRST_GAMEOBJECT_H
#define SDL3_FIRST_GAMEOBJECT_H
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "component.h"



class Shader;
class Component;

class GameObject {
protected:
    inline static std::uint32_t __id_counter = 0;
public:
    std::uint32_t id;
    std::string instance_name = "Game Object" + std::to_string(id);

    std::vector<std::unique_ptr<Component>> components;
    GameObject() : id (__id_counter++) {}
    virtual ~GameObject() = default;

    template<typename T, typename... Args>
    T& add_component(Args&&... args) {
            static_assert(std::is_base_of_v<Component, T>);
            auto c = std::make_unique<T>(std::forward<Args>(args)...);
            c->owner = this;
            T& ref = *c;
            components.push_back(std::move(c));
            return ref;
    }


    void add_component_instance_runtime(Component* comp) {
        comp->owner = this;
        components.push_back(std::unique_ptr<Component>(comp));
    }

    template<typename T>
    T* get_component() {
        for (auto& c : components)
            if (auto ptr = dynamic_cast<T*>(c.get()))
                return ptr;
        return nullptr;
    }

    void update(float dt) const {
        for (auto& c : components)
            c->on_update(dt);
    }

    void render(Shader *shader, Camera camera) const {
        for (auto& c : components)
            c->on_render(shader, camera);
    }


    std::string get_instance_name() {
        return instance_name;
    }

    virtual const char* get_type_name() {
        return "GameObject";
    }


};

class GameObjectRegistry {
    public:
    using Func = std::function<GameObject*()>;

    static GameObjectRegistry& instance() {
        static GameObjectRegistry reg;
        return reg;
    }

    void registerClass(const std::string& name, Func func) {
        entries.push_back({name, func});
    }

    // void callAll() {
    //     for (auto& e : entries) {
    //         std::cout << "Calling " << e.name << "\n";
    //         e.func();
    //     }
    // }

    std::vector<std::string> get_names() {
        std::vector<std::string> names;
        for (auto& e : entries) {
            names.push_back(e.name.c_str());
        }
        return names;
    }

    GameObject* get_game_object_constructor(int index) {
        return entries.at(index).func();
    }

    private:
    struct Entry {
        std::string name;
        Func func;
    };
    std::vector<Entry> entries;
};

struct AutoRegisterGameObject {
    AutoRegisterGameObject(const std::string& name, GameObjectRegistry::Func func) {
        GameObjectRegistry::instance().registerClass(name, func);
    }
};

#define REGISTER_GAME_OBJECT(CLASS) \
namespace { \
AutoRegisterGameObject _autoReg_##CLASS(#CLASS, []() -> GameObject* { return new CLASS(); }); \
}



#endif //SDL3_FIRST_GAMEOBJECT_H