#pragma once
class drawOBJ {
public:
    unsigned int VBO, VAO, EBO;
    std::vector<float> vert;
    std::vector<int> attr;
    std::vector<int> ind;
    bool dynamic = false;
    void bind() {
       // glBindVertexArray(0);
        glBindVertexArray(VAO);
    }
    void create(std::vector<float> vertices, std::vector<int> attributes, std::vector<int> indices ,GLenum test) {
        vert = vertices;
        attr = attributes;
        ind = indices;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        if (!dynamic) {
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), test);
        }
        else {
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), test);
        }


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        if (attributes.size() != 0) {
            std::cout << "HERE1\n";
            for (int i = 0, start = 0; i < attributes.size(); i++) {
                glVertexAttribPointer(i, attributes[i], GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(start * sizeof(float)));
                glEnableVertexAttribArray(i);
                start += attributes[i];
            }
        }

        if (indices.size() != 0) {
            std::cout << "HERE2\n";
            if (!dynamic) {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);
            }
            else {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_DYNAMIC_DRAW);
            }

        }

        glEnableVertexAttribArray(0);
    }

    void draw() {
        if (ind.size() == 0) {
            //https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDrawArrays.xhtml
            glDrawArrays(GL_TRIANGLES, 0, vert.size());// glDrawArrays(GLenum mode, GLint first, GLsizei count);
        }
        else {
            glDrawElements(GL_TRIANGLES, vert.size(), GL_UNSIGNED_INT, 0);
        }
    }
};

