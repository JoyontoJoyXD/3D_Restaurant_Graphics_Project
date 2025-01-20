// main.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "pointLight.h"
#include "directionalLight.h"

#include <iostream>

using namespace std;

bool rotateCeilingFan = false; // Fan rotation state
float ceilingFanRotationAngle = 0.0f; // Fan rotation angle


// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void drawCube(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model, glm::vec3 color);
void drawRestaurant(unsigned int& cubeVAO, Shader& lightingShader);
void drawCeilingFan(unsigned int& cubeVAO, Shader& lightingShader);
void drawTable(unsigned int& cubeVAO, Shader& lightingShader, glm::vec3 position);

void drawChair(unsigned int& cubeVAO, Shader& lightingShader, glm::vec3 position, float rotationAngle);

void drawLightSource(unsigned int& cubeVAO, Shader& lightingShader, glm::vec3 position, glm::vec3 color);
void drawWalls(unsigned int& cubeVAO, Shader& lightingShader);


// Function prototypes for additional features
void drawWallArt(unsigned int& cubeVAO, Shader& lightingShader);
void drawShelf(unsigned int& cubeVAO, Shader& lightingShader);
void drawTableSettings(unsigned int& cubeVAO, Shader& lightingShader, glm::vec3 tablePosition);
void drawPendantLight(unsigned int& cubeVAO, Shader& lightingShader);
void drawWindows(unsigned int& cubeVAO, Shader& lightingShader);


// Add these helper function prototypes
void setAmbientLighting(bool state);
void setDiffuseLighting(bool state);
void setSpecularLighting(bool state);





// Settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

// Camera
Camera camera(glm::vec3(0.0f, 3.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


DirectionalLight directionalLight(
    glm::vec3(-0.2f, -1.0f, -0.3f),  // Direction 
    glm::vec3(0.3f, 0.3f, 0.3f),     //  ambient --intensitity compo.
    glm::vec3(0.5f, 0.5f, 0.5f),     //  diffuse
    glm::vec3(0.5f, 0.5f, 0.5f)      //  specular
);




PointLight pointlight1(
    4.0f, 5.0f, -4.0f,  // Position
    0.6f, 0.3f, 0.6f,   //  ambient
    1.0f, 0.5f, 1.0f,   //  diffuse
    1.0f, 0.5f, 1.0f,   // Specular
    1.0f,                // Constant
    0.09f,               // Linear
    0.032f,              // Quadratic
    1                    // ID
);

PointLight pointlight2(
    -4.0f, 5.0f, -4.0f, // Position
    0.6f, 0.6f, 0.6f,   //  ambient
    1.0f, 1.0f, 1.0f,   //  diffuse
    1.0f, 1.0f, 1.0f,   // Specular
    1.0f,                // Constant
    0.09f,               // Linear
    0.032f,              // Quadratic
    2                    // ID
);


PointLight pointlight3(
    0.0f, 4.0f, 3.0f,   // Position
    0.0f, 0.0f, 0.9f,   // Ambient (emphasize blue)
    0.0f, 0.0f, 2.0f,   // Diffuse (stronger blue)
    0.5f, 0.5f, 1.0f,   // Specular (blue with white highlights)
    1.0f,               // Constant
    0.07f,              // Linear (tweak for brightness)
    0.017f,             // Quadratic (adjust falloff)
    3                   // ID
);












// Render scene
int main()
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Restaurant", NULL, NULL);
    if (!window)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Compile shaders
    Shader lightingShader("vertexShaderForPhongShading.vs", "fragmentShaderForPhongShading.fs");

    // Set up cube VAO
    float cubeVertices[] = {
        // Positions         // Normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f
    };
    unsigned int cubeIndices[] = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        0, 4, 7, 7, 3, 0,
        1, 5, 6, 6, 2, 1,
        0, 1, 5, 5, 4, 0,
        3, 2, 6, 6, 7, 3
    };
    unsigned int cubeVAO, cubeVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        // Setup lighting
        directionalLight.setUpLight(lightingShader);
        pointlight1.setUpPointLight(lightingShader);
        pointlight2.setUpPointLight(lightingShader);
        pointlight3.setUpPointLight(lightingShader); // Activate blue point light

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // Draw restaurant floor
        drawRestaurant(cubeVAO, lightingShader);

        // Draw walls
        drawWalls(cubeVAO, lightingShader);

        // Draw light source cubes
        drawLightSource(cubeVAO, lightingShader, glm::vec3(4.0f, 5.0f, -4.0f), glm::vec3(1.0f, 0.5f, 1.0f)); // Pink light source
        drawLightSource(cubeVAO, lightingShader, glm::vec3(-4.0f, 5.0f, -4.0f), glm::vec3(1.0f, 1.0f, 1.0f)); // White light source
        drawLightSource(cubeVAO, lightingShader, glm::vec3(0.0f, 4.0f, 3.0f), glm::vec3(0.0f, 0.0f, 1.0f)); 


        // Draw pendant light in the dark area
        drawPendantLight(cubeVAO, lightingShader);

        glm::vec3 tablePositions[] = {
        glm::vec3(-3.0f, 0.5f, -3.0f),
        glm::vec3(3.0f, 0.5f, -3.0f),
        glm::vec3(-3.0f, 0.5f, 3.0f),
        glm::vec3(3.0f, 0.5f, 3.0f)
        };

        for (glm::vec3 tablePos : tablePositions)
        {
            drawTable(cubeVAO, lightingShader, tablePos);

            float chairDistance = 1.6f;

            // Draw chairs with backrests positioned at the rear edge
            drawChair(cubeVAO, lightingShader, tablePos + glm::vec3(chairDistance, 0.0f, 0.0f), -90.0f); // Right chair facing center
            drawChair(cubeVAO, lightingShader, tablePos + glm::vec3(-chairDistance, 0.0f, 0.0f), 90.0f); // Left chair facing center
            drawChair(cubeVAO, lightingShader, tablePos + glm::vec3(0.0f, 0.0f, chairDistance), 180.0f); // Back chair facing center
            drawChair(cubeVAO, lightingShader, tablePos + glm::vec3(0.0f, 0.0f, -chairDistance), 0.0f);  // Front chair facing center
        }





        // Inside the render loop, callimg  these functions
        drawWallArt(cubeVAO, lightingShader);
        drawShelf(cubeVAO, lightingShader);
        for (glm::vec3 tablePos : tablePositions)
        {
            drawTable(cubeVAO, lightingShader, tablePos);
            drawTableSettings(cubeVAO, lightingShader, tablePos);
        }
      
        drawWindows(cubeVAO, lightingShader);

        // Draw pendant light in the front part
        drawPendantLight(cubeVAO, lightingShader);




        drawCeilingFan(cubeVAO, lightingShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }




    // Cleanup
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);

    glfwTerminate();
    return 0;
}

// Utility and drawing functions go here...
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    static float lastX = SCR_WIDTH / 2.0f;
    static float lastY = SCR_HEIGHT / 2.0f;
    static bool firstMouse = true;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(static_cast<float>(xoffset), static_cast<float>(yoffset));
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}




void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        // Directional Light Controls
        if (key == GLFW_KEY_B) {
            directionalLight.turnOn();
        }
        else if (key == GLFW_KEY_N) {
            directionalLight.turnOff();
        }

        // Point Light Controls
        if (key == GLFW_KEY_C) {
            pointlight1.turnOn();
            pointlight2.turnOn();
            pointlight3.turnOn();
        }
        else if (key == GLFW_KEY_V) {
            pointlight1.turnOff();
            pointlight2.turnOff();
            pointlight3.turnOff();
        }

        // Ambient Light Controls
        if (key == GLFW_KEY_1) {
            directionalLight.turnAmbientOn();
            pointlight1.turnAmbientOn();
            pointlight2.turnAmbientOn();
            pointlight3.turnAmbientOn();
        }
        else if (key == GLFW_KEY_2) {
            directionalLight.turnAmbientOff();
            pointlight1.turnAmbientOff();
            pointlight2.turnAmbientOff();
            pointlight3.turnAmbientOff();
        }

        // Diffuse Light Controls
        if (key == GLFW_KEY_3) {
            directionalLight.turnDiffuseOn();
            pointlight1.turnDiffuseOn();
            pointlight2.turnDiffuseOn();
            pointlight3.turnDiffuseOn();
        }
        else if (key == GLFW_KEY_4) {
            directionalLight.turnDiffuseOff();
            pointlight1.turnDiffuseOff();
            pointlight2.turnDiffuseOff();
            pointlight3.turnDiffuseOff();
        }

        // Specular Light Controls
        if (key == GLFW_KEY_5) {
            directionalLight.turnSpecularOn();
            pointlight1.turnSpecularOn();
            pointlight2.turnSpecularOn();
            pointlight3.turnSpecularOn();
        }
        else if (key == GLFW_KEY_6) {
            directionalLight.turnSpecularOff();
            pointlight1.turnSpecularOff();
            pointlight2.turnSpecularOff();
            pointlight3.turnSpecularOff();
        }

        // Ceiling Fan Controls
        if (key == GLFW_KEY_J) {
            rotateCeilingFan = true; // Start rotation
        }
        else if (key == GLFW_KEY_K) {
            rotateCeilingFan = false; // Stop rotation
        }
    }
}




void drawCube(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model, glm::vec3 color)
{
    lightingShader.setVec3("material.ambient", color);
    lightingShader.setVec3("material.diffuse", color);
    lightingShader.setVec3("material.specular", glm::vec3(0.5f));
    lightingShader.setFloat("material.shininess", 32.0f);
    lightingShader.setMat4("model", model);

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}


void drawRestaurant(unsigned int& cubeVAO, Shader& lightingShader)
{
    glm::mat4 model = glm::mat4(1.0f);

    // Floor
    model = glm::scale(model, glm::vec3(10.0f, 0.1f, 10.0f));
    drawCube(cubeVAO, lightingShader, model, glm::vec3(0.5f, 0.5f, 0.5f));

    // Debug cube for visibility
    //glm::mat4 debugCube = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //debugCube = glm::scale(debugCube, glm::vec3(1.0f));
    //drawCube(cubeVAO, lightingShader, debugCube, glm::vec3(1.0f, 0.0f, 0.0f)); // Red cube for debugging

    drawCube(cubeVAO, lightingShader, glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 0.1f, 10.0f)), glm::vec3(0.6f, 0.6f, 0.6f));

}


void drawCeilingFan(unsigned int& cubeVAO, Shader& lightingShader)
{
    // Draw the base of the ceiling fan
    glm::mat4 baseModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.8f, 0.0f));
    baseModel = glm::scale(baseModel, glm::vec3(0.2f, 0.6f, 0.2f)); // Adjust dimensions for the base rod
    drawCube(cubeVAO, lightingShader, baseModel, glm::vec3(0.5f, 0.2f, 0.8f)); // Dark gray base

    // Draw the motor housing of the ceiling fan
    glm::mat4 motorModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.5f, 0.0f));
    motorModel = glm::scale(motorModel, glm::vec3(0.5f, 0.2f, 0.5f)); // Circular-like motor casing
    drawCube(cubeVAO, lightingShader, motorModel, glm::vec3(1.0f, 1.0f, 1.0f)); // Light gray motor casing

    // Update rotation angle if the fan is rotating
    if (rotateCeilingFan)
    {
        ceilingFanRotationAngle += 700.0f * deltaTime; // Rotation speed
        if (ceilingFanRotationAngle >= 360.0f) ceilingFanRotationAngle = 0.0f; // Keep angle within 360 degrees
    }

    // Draw the fan blades
    for (int i = 0; i < 4; ++i)
    {
        glm::mat4 bladeModel = glm::translate(motorModel, glm::vec3(0.0f, -0.1f, 0.0f)); // Slightly below the motor
        bladeModel = glm::rotate(bladeModel, glm::radians(ceilingFanRotationAngle + 90.0f * i), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate blades
        bladeModel = glm::translate(bladeModel, glm::vec3(0.0f, 0.0f, 1.0f)); // Extend outward
        bladeModel = glm::scale(bladeModel, glm::vec3(0.5f, 0.2f, 5.0f)); // Thin and long blades
        drawCube(cubeVAO, lightingShader, bladeModel, glm::vec3(0.8f, 0.2f, 0.2f)); // Red blades
    }
}


void drawTable(unsigned int& cubeVAO, Shader& lightingShader, glm::vec3 position)
{
    // Draw the tabletop
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, glm::vec3(2.0f, 0.1f, 2.0f)); // Thin tabletop
    drawCube(cubeVAO, lightingShader, model, glm::vec3(0.6f, 0.3f, 0.1f)); // Wood-like color

    // Adjust leg offsets relative to the tabletop size
    glm::vec3 legOffsets[] = {
        glm::vec3(-0.42f, -0.42f, -0.42f), // Front-left leg
        glm::vec3(0.42f, -0.42f, -0.42f),  // Front-right leg
        glm::vec3(-0.42f, -0.42f, 0.42f),  // Back-left leg
        glm::vec3(0.42f, -0.42f, 0.42f)    // Back-right leg
    };

    glm::vec3 legScale = glm::vec3(0.1f, 0.6f, 0.1f); // Leg dimensions

    // Correct leg positioning based on tabletop scaling
    for (int i = 0; i < 4; ++i)
    {
        glm::mat4 legModel = glm::translate(glm::mat4(1.0f), position + glm::vec3(legOffsets[i].x * 2.0f, -0.28f, legOffsets[i].z * 2.0f));
        legModel = glm::scale(legModel, legScale); // Scale the legs
        drawCube(cubeVAO, lightingShader, legModel, glm::vec3(0.5f, 0.2f, 0.1f)); // Leg color
    }
}


void drawChair(unsigned int& cubeVAO, Shader& lightingShader, glm::vec3 position, float rotationAngle)
{
    // Chair seat
    glm::mat4 seatModel = glm::translate(glm::mat4(1.0f), position);
    seatModel = glm::rotate(seatModel, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f)); // Apply rotation
    seatModel = glm::translate(seatModel, glm::vec3(0.0f, -0.15f, 0.0f)); // Adjust position after rotation
    seatModel = glm::scale(seatModel, glm::vec3(0.5f, 0.1f, 0.5f)); // Dimensions of the seat
    drawCube(cubeVAO, lightingShader, seatModel, glm::vec3(0.4f, 0.2f, 0.1f)); // Brown seat

    // Chair legs
    glm::vec3 legOffsets[] = {
        glm::vec3(-0.2f, -0.25f, -0.2f), // Front-left
        glm::vec3(0.2f, -0.25f, -0.2f),  // Front-right
        glm::vec3(-0.2f, -0.25f, 0.2f),  // Back-left
        glm::vec3(0.2f, -0.25f, 0.2f)    // Back-right
    };

    for (int i = 0; i < 4; ++i)
    {
        glm::mat4 legModel = glm::translate(glm::mat4(1.0f), position);
        legModel = glm::rotate(legModel, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f)); // Apply rotation
        legModel = glm::translate(legModel, legOffsets[i]);
        legModel = glm::scale(legModel, glm::vec3(0.05f, 0.25f, 0.05f)); // Legs' dimensions
        drawCube(cubeVAO, lightingShader, legModel, glm::vec3(0.4f, 0.2f, 0.1f)); // Brown legs
    }

    // Chair backrest (Positioned at the edge of the seat)
    glm::mat4 backrestModel = glm::translate(glm::mat4(1.0f), position);
    backrestModel = glm::rotate(backrestModel, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f)); // Apply rotation
    backrestModel = glm::translate(backrestModel, glm::vec3(0.0f, 0.2f, -0.25f - 0.05f)); // Move backrest to edge
    backrestModel = glm::scale(backrestModel, glm::vec3(0.5f, 0.6f, 0.1f)); // Taller and thinner backrest
    drawCube(cubeVAO, lightingShader, backrestModel, glm::vec3(0.4f, 0.2f, 0.1f)); // Brown backrest
}







void drawLightSource(unsigned int& cubeVAO, Shader& lightingShader, glm::vec3 position, glm::vec3 color)
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position); // Place the light source
    model = glm::scale(model, glm::vec3(0.3f)); // Adjust the size of the light cube
    lightingShader.setMat4("model", model);

    // Set cube color to match the light source
    lightingShader.setVec3("material.ambient", color); // Ambient color
    lightingShader.setVec3("material.diffuse", color); // Diffuse color
    lightingShader.setVec3("material.specular", glm::vec3(1.0f)); // Specular highlight
    lightingShader.setFloat("material.shininess", 32.0f); // Shininess

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}


void drawWalls(unsigned int& cubeVAO, Shader& lightingShader)
{
    glm::mat4 wallModel;

    // Set wall material properties
    glm::vec3 wallColor = glm::vec3(0.9f, 0.9f, 0.9f); // Off-white color
    glm::vec3 specular = glm::vec3(0.1f); // Reduced specular reflection
    float shininess = 16.0f; // Reduced shininess

    lightingShader.setVec3("material.specular", specular);
    lightingShader.setFloat("material.shininess", shininess);

    // Left Wall
    wallModel = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 2.5f, 0.0f));
    wallModel = glm::scale(wallModel, glm::vec3(0.1f, 5.0f, 10.0f));
    drawCube(cubeVAO, lightingShader, wallModel, wallColor);

    // Right Wall
    wallModel = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 2.5f, 0.0f));
    wallModel = glm::scale(wallModel, glm::vec3(0.1f, 5.0f, 10.0f));
    drawCube(cubeVAO, lightingShader, wallModel, wallColor);

    // Back Wall
    wallModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f, -5.0f));
    wallModel = glm::scale(wallModel, glm::vec3(10.0f, 5.0f, 0.1f));
    drawCube(cubeVAO, lightingShader, wallModel, wallColor);

    // Ceiling
    wallModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, 0.0f));
    wallModel = glm::scale(wallModel, glm::vec3(10.0f, 0.1f, 10.0f));
    drawCube(cubeVAO, lightingShader, wallModel, wallColor);
}







void drawWallArt(unsigned int& cubeVAO, Shader& lightingShader)
{
    glm::mat4 model;

    // Wall Art (painting on the left wall)
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.9f, 2.5f, -2.0f)); // Slightly away from the left wall
    model = glm::scale(model, glm::vec3(0.1f, 1.5f, 2.0f)); // Thin rectangular art piece
    drawCube(cubeVAO, lightingShader, model, glm::vec3(0.7f, 0.2f, 0.2f)); // Dark red color for the painting
}


void drawShelf(unsigned int& cubeVAO, Shader& lightingShader)
{
    glm::mat4 model;

    // Shelf (on the left wall)
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.9f, 1.5f, 1.0f)); // Positioned at mid-height
    model = glm::scale(model, glm::vec3(0.1f, 0.2f, 2.0f)); // Thin horizontal shelf
    drawCube(cubeVAO, lightingShader, model, glm::vec3(0.4f, 0.2f, 0.1f)); // Wood color for the shelf

    // Decorative Item (book)
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.7f, 1.6f, 1.0f)); // On the shelf
    model = glm::scale(model, glm::vec3(0.1f, 0.4f, 0.2f)); // Thin book-like object
    drawCube(cubeVAO, lightingShader, model, glm::vec3(0.1f, 0.1f, 0.8f)); // Blue color for the book
}



void drawTableSettings(unsigned int& cubeVAO, Shader& lightingShader, glm::vec3 tablePosition)
{
    glm::mat4 model;

    // Plate (Touching the table surface)
    model = glm::translate(glm::mat4(1.0f), tablePosition + glm::vec3(0.3f, 0.05f, 0.3f)); // Adjusted height to be on the table surface
    model = glm::scale(model, glm::vec3(0.3f, 0.02f, 0.3f)); // Flat circular-like object
    drawCube(cubeVAO, lightingShader, model, glm::vec3(0.9f, 0.9f, 0.9f)); // White color for the plate

    // Glass (Touching the table surface)
    model = glm::translate(glm::mat4(1.0f), tablePosition + glm::vec3(-0.3f, 0.1f, 0.3f)); // Adjusted height to place the base of the glass on the table
    model = glm::scale(model, glm::vec3(0.1f, 0.2f, 0.1f)); // Cylindrical glass-like object
    drawCube(cubeVAO, lightingShader, model, glm::vec3(0.8f, 0.8f, 1.0f)); // Slightly transparent blue glass

    // Napkin (Touching the table surface)
    model = glm::translate(glm::mat4(1.0f), tablePosition + glm::vec3(0.0f, 0.05f, -0.3f)); // Adjusted height to lie flat on the table
    model = glm::scale(model, glm::vec3(0.2f, 0.01f, 0.2f)); // Thin flat square
    drawCube(cubeVAO, lightingShader, model, glm::vec3(1.0f, 1.0f, 1.0f)); // White napkin
}




void drawPendantLight(unsigned int& cubeVAO, Shader& lightingShader)
{
    glm::mat4 model;

    // Decorative Pendant Light Base
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.0f, 3.0f)); // Positioned in the front dark area
    model = glm::scale(model, glm::vec3(0.4f, 0.6f, 0.4f)); // Slightly larger and rounded
    drawCube(cubeVAO, lightingShader, model, glm::vec3(0.2f, 0.2f, 0.8f)); // Dark blue metallic structure

    // Glowing Glass Bulb
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.7f, 3.0f)); // Positioned slightly below the base
    model = glm::scale(model, glm::vec3(0.3f, 0.4f, 0.3f)); // Bulb size
    drawCube(cubeVAO, lightingShader, model, glm::vec3(0.3f, 0.7f, 1.0f)); // Bright translucent light blue for the bulb

    // Add a ring detail at the bottom of the base
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.2f, 3.0f)); // At the bottom of the base
    model = glm::scale(model, glm::vec3(0.5f, 0.05f, 0.5f)); // Thin decorative ring
    drawCube(cubeVAO, lightingShader, model, glm::vec3(0.4f, 0.4f, 1.0f)); // Slightly lighter blue for contrast

    // Chandelier-style chain (alternating spheres and cylinders)
    glm::vec3 chainColor = glm::vec3(0.8f, 0.6f, 0.3f); // Gold chain color
    float chainStartHeight = 4.8f; // Starting point for the chain (closer to the ceiling)
    float linkSpacing = 0.2f;
    float chainEndHeight = 4.0f; // The base of the pendant light

    for (float offset = 0.0f; offset <= (chainStartHeight - chainEndHeight); offset += linkSpacing)
    {
        // Sphere (decorative chain links)
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, chainStartHeight - offset, 3.0f));
        model = glm::scale(model, glm::vec3(0.08f)); // Small sphere for the chain
        drawCube(cubeVAO, lightingShader, model, chainColor);

        // Cylinder (connecting parts of the chain)
        if (offset + linkSpacing < (chainStartHeight - chainEndHeight))
        {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, chainStartHeight - offset - linkSpacing / 2.0f, 3.0f));
            model = glm::scale(model, glm::vec3(0.05f, linkSpacing / 2.0f, 0.05f)); // Thin cylinder connecting links
            drawCube(cubeVAO, lightingShader, model, chainColor);
        }
    }

    // Ceiling Plate (where the chain attaches)
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, chainStartHeight + 0.05f, 3.0f)); // Slightly above the chain
    model = glm::scale(model, glm::vec3(0.3f, 0.05f, 0.3f)); // Thin plate
    drawCube(cubeVAO, lightingShader, model, glm::vec3(0.6f, 0.6f, 0.6f)); // Neutral gray for the plate

    // Emit Light Source Visualization (The actual light glow)
    glm::vec3 lightColor = glm::vec3(0.0f, 0.5f, 1.0f); // Vibrant blue light color
    drawLightSource(cubeVAO, lightingShader, glm::vec3(0.0f, 3.7f, 3.0f), lightColor); // Positioned at the bulb
}




void drawWindows(unsigned int& cubeVAO, Shader& lightingShader)
{
    glm::mat4 model;

    // Window frame on the right wall
    model = glm::translate(glm::mat4(1.0f), glm::vec3(4.9f, 3.0f, 0.0f)); // Centered on the right wall
    model = glm::scale(model, glm::vec3(0.1f, 1.5f, 2.0f)); // Thin vertical window
    drawCube(cubeVAO, lightingShader, model, glm::vec3(0.8f, 0.8f, 1.0f)); // Light blue for the glass

    // Left Curtain
    model = glm::translate(glm::mat4(1.0f), glm::vec3(4.8f, 3.0f, -1.0f)); // Left side of the window
    model = glm::scale(model, glm::vec3(0.1f, 1.5f, 0.5f)); // Thin vertical rectangle
    drawCube(cubeVAO, lightingShader, model, glm::vec3(0.7f, 0.3f, 0.3f)); // Red curtain

    // Right Curtain
    model = glm::translate(glm::mat4(1.0f), glm::vec3(4.8f, 3.0f, 1.0f)); // Right side of the window
    model = glm::scale(model, glm::vec3(0.1f, 1.5f, 0.5f)); // Thin vertical rectangle
    drawCube(cubeVAO, lightingShader, model, glm::vec3(0.7f, 0.3f, 0.3f)); // Red curtain
}


void setAmbientLighting(bool state)
{
    if (state) {
        directionalLight.turnAmbientOn();
        pointlight1.turnAmbientOn();
        pointlight2.turnAmbientOn();
        pointlight3.turnAmbientOn();
    }
    else {
        directionalLight.turnAmbientOff();
        pointlight1.turnAmbientOff();
        pointlight2.turnAmbientOff();
        pointlight3.turnAmbientOff();
    }
}

void setDiffuseLighting(bool state)
{
    if (state) {
        directionalLight.turnDiffuseOn();
        pointlight1.turnDiffuseOn();
        pointlight2.turnDiffuseOn();
        pointlight3.turnDiffuseOn();
    }
    else {
        directionalLight.turnDiffuseOff();
        pointlight1.turnDiffuseOff();
        pointlight2.turnDiffuseOff();
        pointlight3.turnDiffuseOff();
    }
}

void setSpecularLighting(bool state)
{
    if (state) {
        directionalLight.turnSpecularOn();
        pointlight1.turnSpecularOn();
        pointlight2.turnSpecularOn();
        pointlight3.turnSpecularOn();
    }
    else {
        directionalLight.turnSpecularOff();
        pointlight1.turnSpecularOff();
        pointlight2.turnSpecularOff();
        pointlight3.turnSpecularOff();
    }
}

