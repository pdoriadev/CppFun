#include "MatrixStack.h"

MatrixStack::MatrixStack() {

    // Initialize the matrix stack with the identity matrix.
    //{1,0,0,0,1,0,0,0,1}
    // m_matrices.push_back(Matrix4f(1.0f, 0.0f, 0.0f, 0.0f,
    // 							  0.0f, 1.0f, 0.0f, 0.0f,
    // 							  0.0f, 0.0f, 1.0f, 0.0f,
    // 							  0.0f, 0.0f, 0.0f, 1.0f));
	
    m_matrices.push_back(glm::mat4(1.0f));
	
}

void MatrixStack::clear() {

    // Revert to just containing the identity matrix.
    m_matrices.clear();
    // m_matrices.push_back(Matrix4f(1.0f, 0.0f, 0.0f, 0.0f,
    // 							  0.0f, 1.0f, 0.0f, 0.0f,
    // 							  0.0f, 0.0f, 1.0f, 0.0f,
    // 							  0.0f, 0.0f, 0.0f, 1.0f));	

    m_matrices.clear();
    m_matrices.push_back(glm::mat4(1.0f));

}

glm::mat4 MatrixStack::top() const {

    // Return the top of the stack
    
    return m_matrices.back();
}

void MatrixStack::push(const glm::mat4& m) {

    // Push m onto the stack.
    // Your stack should have OpenGL semantics:
    // the new top should be the old top multiplied by m
	
    glm::mat4 stack = m_matrices.back() * m;
    m_matrices.push_back(stack);

}

void MatrixStack::pop() {

    // Remove the top element from the stack (ensure stack isn't empty)
    if (m_matrices.size() > 1) {
        m_matrices.pop_back();
    }
}

