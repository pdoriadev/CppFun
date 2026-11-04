#ifndef MATRIXSTACK_H
#define MATRIXSTACK_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>

class MatrixStack {
public:
    MatrixStack();
    void clear();
    glm::mat4 top() const;
    void push(const glm::mat4& m);
    void pop();

private:
    std::vector<glm::mat4> m_matrices;
};

#endif // MATRIX_STACK_H

