#pragma once
#include <algorithm>
#include "structs.h"

class drawOBJ {
    private:
        GLuint VBO, VAO;
    public:
        
        float vert[16] = {
           -1.0f, 1.0f,   0.0f, 1.0f,
           -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,   0.34f, 0.0f,
            1.0f, 1.0f,    0.34f, 1.0f
        };

        int currentframe = 0;
        int maxframe = 2;
        int frameSpeed = 0.25;
        GLfloat frameSizeWidth = .333333333;
        std::vector<int> frames = {};

        std::vector<int> ind;

        void create() {
            ind = { 0, 1, 3, 1, 2, 3 };

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);

            glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_DYNAMIC_DRAW);

            //vec cord attribute
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            //texture cord attribute
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            //clear buffers that are selected
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        void updateTexturePos() {
            currentframe++;
            if (currentframe >= maxframe) {
                currentframe = 0;
            }
            vert[2] =  currentframe* frameSizeWidth;
            vert[6] =  currentframe* frameSizeWidth;
            vert[10] =  (currentframe * frameSizeWidth) + frameSizeWidth;
            vert[14] =   (currentframe * frameSizeWidth) + frameSizeWidth;
            printf("%f : %f\n", vert[2], vert[14]);
        }
        
        void setData(float newVert[16]) {
            for (int i = 0; i < 16; i++) {
                vert[i] = newVert[i];
            }
         }
        void draw() {
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vert), vert );
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDrawElements(GL_TRIANGLES, (ind.size()), GL_UNSIGNED_INT, ind.data());
        }
};  

