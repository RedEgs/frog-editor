//
// Created by User on 03/02/2026.
//

#ifndef SDL3_FIRST_UNIFORM_BUFFER_OBJECT_H
#define SDL3_FIRST_UNIFORM_BUFFER_OBJECT_H
#include <any>
#include <glad/glad.h>

class UniformBufferObject {
public:
    unsigned int ID;

    UniformBufferObject(unsigned int size, unsigned int index) {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_UNIFORM_BUFFER, ID);
        glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW); // allocate 152 bytes of memory


        glBindBufferBase(GL_UNIFORM_BUFFER, index, ID);
    }
    template<typename T>
    void set_object(unsigned int offset, const T value) {
        glBindBuffer(GL_UNIFORM_BUFFER, ID);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(T), &value);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
};


#endif //SDL3_FIRST_UNIFORM_BUFFER_OBJECT_H