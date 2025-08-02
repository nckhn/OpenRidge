#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

unsigned int compileShader(unsigned int type, const char* source);
unsigned int createShaderProgram(const char* vertexSrc, const char* fragmentSrc);

unsigned int loadTexture(const char* path);

float cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,  0, 0, -1,           0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0, 0, -1,           1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0, 0, -1,           1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0, 0, -1,           1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0, 0, -1,           0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0, 0, -1,           0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0, 0, 1,            0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0, 0, 1,            1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0, 0, 1,            1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0, 0, 1,            1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0, 0, 1,            0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0, 0, 1,            0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1, 0, 0,            1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1, 0, 0,            1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1, 0, 0,            0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1, 0, 0,            0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1, 0, 0,            0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1, 0, 0,            1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1, 0, 0,            1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1, 0, 0,            1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1, 0, 0,            0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1, 0, 0,            0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1, 0, 0,            0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1, 0, 0,            1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0, -1, 0,           0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0, -1, 0,           1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0, -1, 0,           1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0, -1, 0,           1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0, -1, 0,           0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0, -1, 0,           0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0, 1, 0,            0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0, 1, 0,            1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0, 1, 0,            1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0, 1, 0,            1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0, 1, 0,            0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0, 1, 0,            0.0f, 1.0f
};

float planeVertices[] = {
     5.0f, 0.0f,  5.0f,  0, 1, 0,        5.0f, 0.0f,
    -5.0f, 0.0f,  5.0f,  0, 1, 0,        0.0f, 0.0f,
    -5.0f, 0.0f, -5.0f,  0, 1, 0,        0.0f, 5.0f,

     5.0f, 0.0f,  5.0f,  0, 1, 0,        5.0f, 0.0f,
    -5.0f, 0.0f, -5.0f,  0, 1, 0,        0.0f, 5.0f,
     5.0f, 0.0f, -5.0f,  0, 1, 0,        5.0f, 5.0f
};

const char* vertexShaderSource = R"glsl(
#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture1;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 ambientColor;

void main()
{
    vec3 color = texture(texture1, TexCoords).rgb;
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    // Ambient
    vec3 ambient = ambientColor * color;

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * color;

    // Specular (Blinn-Phong)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
)glsl";

glm::vec3 carPos(0.f, 0.5f, 0.f);
glm::vec3 carVelocity(0.f);
float carOrientation = 0.f;
float carAngularVelocity = 0.f;

float throttle = 0.f;
float steeringInput = 0.f;

const float maxSpeed = 12.f;
const float maxAcceleration = 25.f;
const float maxSteerAngle = glm::radians(35.f);
const float maxAngularVelocity = glm::radians(120.f);

float deltaTime = 0.f, lastFrame = 0.f;

glm::vec3 cameraPos, cameraTarget, cameraUp(0, 1, 0);

void processInput(GLFWwindow* window)
{
    throttle = 0.f;
    steeringInput = 0.f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) throttle = 1.f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) throttle = -0.5f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) steeringInput = 1.f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) steeringInput = -1.f;
}

void updatePhysics(float dt)
{
    float acceleration = throttle * maxAcceleration;
    carVelocity += glm::vec3(sin(carOrientation), 0.f, cos(carOrientation)) * acceleration * dt;

    float speed = glm::length(carVelocity);
    if (speed > maxSpeed) carVelocity = glm::normalize(carVelocity) * maxSpeed;

    carVelocity *= 0.98f;

    glm::vec3 forwardDir = glm::vec3(sin(carOrientation), 0.f, cos(carOrientation));
    glm::vec3 rightDir = glm::vec3(forwardDir.z, 0.f, -forwardDir.x);
    float lateralSpeed = glm::dot(carVelocity, rightDir);
    carVelocity -= rightDir * lateralSpeed * 0.8f;

    float angularAcceleration = 0.f;
    if (speed > 0.1f)
        angularAcceleration = steeringInput * maxAngularVelocity * (speed / maxSpeed);

    carAngularVelocity += angularAcceleration * dt;
    carAngularVelocity *= 0.7f;

    carOrientation += carAngularVelocity * dt;

    carPos += carVelocity * dt;
}

void updateCamera()
{
    glm::vec3 carForward = glm::vec3(sin(carOrientation), 0, cos(carOrientation));
    glm::vec3 carRight = glm::vec3(carForward.z, 0, -carForward.x);

    glm::vec3 desiredPos = carPos - carForward * 6.f + glm::vec3(0, 3.f, 0) + carRight * 0.5f * steeringInput;

    static glm::vec3 currentPos = desiredPos;
    currentPos = glm::mix(currentPos, desiredPos, 5.f * deltaTime);

    cameraPos = currentPos;
    cameraTarget = carPos + glm::vec3(0, 0.7f, 0);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenRidge", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

 //   unsigned int carTexture = loadTexture("car_texture.png"); 
//    unsigned int trackTexture = loadTexture("track_texture.png");

    lastFrame = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        updatePhysics(deltaTime);
        updateCamera();

        glClearColor(0.53f, 0.81f, 0.92f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), 0.f, 10.f, 10.f);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.f, 1.f, 1.f);
        glUniform3f(glGetUniformLocation(shaderProgram, "ambientColor"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);

        glm::mat4 projection = glm::perspective(glm::radians(60.f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);

        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
  //      glBindTexture(GL_TEXTURE_2D, trackTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(cubeVAO);
  //      glBindTexture(GL_TEXTURE_2D, carTexture);

        model = glm::mat4(1.f);
        model = glm::translate(model, carPos);
        model = glm::rotate(model, carOrientation, glm::vec3(0, 1, 0));

        float rollAngle = steeringInput * glm::radians(8.f) * (glm::length(carVelocity) / maxSpeed);
        model = glm::rotate(model, rollAngle, glm::vec3(0, 0, 1));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

unsigned int compileShader(unsigned int type, const char* source)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error:\n" << infoLog << std::endl;
    }
    return shader;
}

unsigned int createShaderProgram(const char* vertexSrc, const char* fragmentSrc)
{
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Program linking error:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

unsigned int loadTexture(const char* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (data)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load texture at " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}
