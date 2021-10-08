#pragma once
//https://learnopengl.com/
//https://www.geeksforgeeks.org/passing-vector-function-cpp/
class drawOBJ {
public:
    unsigned int VBO, VAO, DATA_POINTS = 4;

    std::vector<float> vertT;
    std::vector<int> attr;
    std::vector<int> ind;

    void create(std::vector<float> vertices, std::vector<int> indices = {}) {
        vertT = vertices;
        ind = indices;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertT.size(), vertT.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, DATA_POINTS, GL_FLOAT, GL_FALSE, sizeof(float) * DATA_POINTS, (void*)0);
        
        //clear buffers that are selected
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    void setData(std::vector<float> vertices) {
        vertT = vertices;
    }
    void setIndices(std::vector<int> indices) {
        ind = indices;
    }
    void setDataPoints(unsigned noPoints) {
        DATA_POINTS = noPoints;
    }
    void bindVao() {
        glBindVertexArray(VAO);
    }
    void draw() {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertT.size(),  vertT.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        if (ind.size() == 0) {
            glDrawArrays(GL_TRIANGLES, 0, (vertT.size() / DATA_POINTS));
        } else {
            glDrawElements(GL_TRIANGLES, (ind.size()  ), GL_UNSIGNED_INT, ind.data());
        }
    }
};

