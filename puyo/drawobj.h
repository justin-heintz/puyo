#pragma once

class drawOBJ {
public:
    unsigned int VBO, VAO, EBO;
    std::vector<float> vert = {};
    std::vector<int> attr;
    std::vector<int> ind;

    void bind() {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vert.size() * 4, NULL, GL_DYNAMIC_DRAW);
        glBindVertexArray(VAO);
    }
    void create(std::vector<float> vertices[], std::vector<int> indices = {}, GLenum type = GL_DYNAMIC_DRAW) {
       // vert = vertices;
        ind = indices;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vert.size() * 4, NULL, type);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

        if (indices.size() != 0) {
          glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), type);
        }

        glEnableVertexAttribArray(0);
    }

    void draw() {
        if (ind.size() == 0) {
            glDrawArrays(GL_TRIANGLES, 0, vert.size());// glDrawArrays(GLenum mode, GLint first, GLsizei count);
        } else {
            glDrawElements(GL_TRIANGLES, vert.size(), GL_UNSIGNED_INT, 0);
        }
    }
};

