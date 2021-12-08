#pragma once
#include <algorithm>
class drawOBJ {
public:
    unsigned int VBO, VAO;
    float vert[20] = {
       -1.0f, 1.0f, 0.0f,  -1.0f, 1.0f,
       -1.0f, -1.0f, 0.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,   0.0f, 1.0f
    };
    std::vector<int> ind;
    std::vector<float> text;
    drawOBJ(){
        ind = { 0, 1, 3, 1, 2, 3 };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_DYNAMIC_DRAW);

        //vec cord attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        //texture cord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        //clear buffers that are selected
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    void setData(float newVert[]) {
        std::copy(std::begin(vert), std::end(vert), newVert);
    }
    void draw() {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vert), vert );
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawElements(GL_TRIANGLES, (ind.size()  ), GL_UNSIGNED_INT, ind.data());
    }
};  

