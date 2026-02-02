    //
    // Created by Charlie on 08/01/2026.
    //

    #ifndef SDL3_FIRST_VERTEX_H
    #define SDL3_FIRST_VERTEX_H
    #include <glm/vec2.hpp>
    #include <glm/vec3.hpp>


    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 tex_coords;
    };



    #endif //SDL3_FIRST_VERTEX_H