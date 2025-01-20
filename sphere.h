#ifndef SPHERE_H
#define SPHERE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "shader.h"


class Sphere {
public:
    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    Sphere(float radius = 1.0f, int sectorCount = 36, int stackCount = 18) {
        generateSphere(radius, sectorCount, stackCount);
        setupMesh();
    }

    void draw(Shader& shader, glm::mat4 model) {
        shader.use();
        shader.setMat4("model", model);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    void generateSphere(float radius, int sectorCount, int stackCount) {
        const float PI = 3.14159265358979323846f;
        for (int i = 0; i <= stackCount; ++i) {
            float stackAngle = PI / 2 - i * PI / stackCount; // From pi/2 to -pi/2
            float xy = radius * cosf(stackAngle);
            float z = radius * sinf(stackAngle);

            for (int j = 0; j <= sectorCount; ++j) {
                float sectorAngle = j * 2 * PI / sectorCount;

                float x = xy * cosf(sectorAngle);
                float y = xy * sinf(sectorAngle);
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                float nx = x / radius;
                float ny = y / radius;
                float nz = z / radius;
                vertices.push_back(nx);
                vertices.push_back(ny);
                vertices.push_back(nz);
            }
        }

        for (int i = 0; i < stackCount; ++i) {
            int k1 = i * (sectorCount + 1);
            int k2 = k1 + sectorCount + 1;

            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
                if (i != 0) {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }
                if (i != (stackCount - 1)) {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }
    }

    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
};

#endif
