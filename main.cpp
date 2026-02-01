#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <cstring>
#include <tinyexpr.h>
#include <cmath>
#include <sstream>
#include <algorithm>

const char* VS = R"(
#version 330 core
layout(location=0) in vec2 aPos;
void main()
{
    gl_Position = vec4(aPos, 0.0f, 1.0f);
}
)";

const char* FS = R"(
#version 330 core
out vec4 fragColor;
void main()
{
    fragColor = vec4(0.02f, 0.02f, 0.05f, 1.0f); // Dark blue background
}
)";

const char* LineFS = R"(
#version 330 core
out vec4 fragColor;
void main()
{
    fragColor = vec4(0.7f, 0.7f, 0.8f, 0.6f); // Light gray axes
}
)";

const char* GridFS = R"(
#version 330 core
out vec4 fragColor;
void main()
{
    fragColor = vec4(0.3f, 0.3f, 0.4f, 0.2f); // Subtle grid lines
}
)";

const char* graphVS = R"(
#version 330 core
layout(location=0) in vec2 aPos;
void main()
{
 gl_Position = vec4(aPos, 0.0f, 1.0f);
}
)";

const char* graphFS = R"(
#version 330 core
out vec4 fragColor;
void main()
{
    fragColor = vec4(0.0f, 1.0f, 0.8f, 1.0f); //Neon Electric Green
}
)";

// Compile Shader
GLuint compileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader Compilation Error: " << log << std::endl;
    }
    return shader;
}

// Create Shader Program
GLuint createProgram(GLuint vs, GLuint fs)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // Check linking
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Program Linking Error: " << log << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

int main()
{
    te_expr* expr = nullptr;
    double x, y;
        // Normal function
        std::string mathExpression;
        std::cout << "\nEnter the function f(x): ";
        std::getline(std::cin, mathExpression);

        te_variable vars[] = { { "x", &x } };
        int err;
        expr = te_compile(mathExpression.c_str(), vars, 1, &err);
        if (!expr) {
            std::cerr << "\nFailure in conversion of expression!" << std::endl;
            return -1;
        }


    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Calculus in Motion", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    glEnable(GL_MULTISAMPLE);

    // Compile shaders
    GLuint vs1 = compileShader(GL_VERTEX_SHADER, VS);
    GLuint vs2 = compileShader(GL_VERTEX_SHADER, VS);
    GLuint vs3 = compileShader(GL_VERTEX_SHADER, VS);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, FS);
    GLuint linefs = compileShader(GL_FRAGMENT_SHADER, LineFS);
    GLuint gridfs = compileShader(GL_FRAGMENT_SHADER, GridFS);

    GLuint sp = createProgram(vs1, fs);        // Background
    GLuint linesp = createProgram(vs2, linefs); // Axes
    GLuint gridsp = createProgram(vs3, gridfs); // Grid

    // Full screen quad
    float quadVerts[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Axes
    float lines[] = {
        // y-axis
        0.0f, -1.0f,
        0.0f,  1.0f,
        // x-axis
       -1.0f,  0.0f,
        1.0f,  0.0f
    };

    GLuint LineVAO, LineVBO;
    glGenVertexArrays(1, &LineVAO);
    glGenBuffers(1, &LineVBO);

    glBindVertexArray(LineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, LineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Grid lines
    std::vector<float> gridVertices;
    int gridLines = 10;
    float domainScale = 10.0f;
    float rangeScale = 5.8f;

    for (int i = -gridLines; i <= gridLines; i++) {
        if (i == 0) continue;

        float x_pos = (i / (float)gridLines) * (domainScale / domainScale);
        float y_pos = (i / (float)gridLines) * (domainScale / rangeScale);

        // Vertical grid lines
        gridVertices.push_back(x_pos);
        gridVertices.push_back(-1.0f);
        gridVertices.push_back(x_pos);
        gridVertices.push_back(1.0f);

        // Horizontal grid lines  
        gridVertices.push_back(-1.0f);
        gridVertices.push_back(y_pos);
        gridVertices.push_back(1.0f);
        gridVertices.push_back(y_pos);
    }

    GLuint GridVAO, GridVBO;
    glGenVertexArrays(1, &GridVAO);
    glGenBuffers(1, &GridVBO);

    glBindVertexArray(GridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, GridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Ticks
    std::vector<float> tickVertices;
    float tickSize = 0.02f;

    for (int i = -gridLines; i <= gridLines; i++) {
        if (i == 0) continue;

        float x_pos = (i / (float)gridLines) * (domainScale / domainScale);
        float y_pos = (i / (float)gridLines) * (domainScale / rangeScale);

        // X-axis ticks
        tickVertices.push_back(x_pos);
        tickVertices.push_back(-tickSize);
        tickVertices.push_back(x_pos);
        tickVertices.push_back(tickSize);

        // Y-axis ticks
        tickVertices.push_back(-tickSize);
        tickVertices.push_back(y_pos);
        tickVertices.push_back(tickSize);
        tickVertices.push_back(y_pos);
    }

    GLuint TickVAO, TickVBO;
    glGenVertexArrays(1, &TickVAO);
    glGenBuffers(1, &TickVBO);

    glBindVertexArray(TickVAO);
    glBindBuffer(GL_ARRAY_BUFFER, TickVBO);
    glBufferData(GL_ARRAY_BUFFER, tickVertices.size() * sizeof(float), tickVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Generate graph points WITH NOISE FILTERING - MODIFIED FOR DIFFERENT FUNCTION TYPES
    std::vector<std::pair<float, float>> coords;
    int numPoints = 50000;

    float prevY = 0.0f;
    bool firstPoint = true;
    const float MAX_JUMP = 2.0f;

    for (int i = 0; i < numPoints; ++i)
    {
        float screenX = -1.0f + 2.0f * i / (numPoints - 1);
        x = screenX * domainScale;
        double y_val = 0.0;
        bool validPoint = true;

            // Normal function
            y_val = te_eval(expr);

        float screenY = y_val / rangeScale;

        // Filter out noise and discontinuities
        if (validPoint) {
            if (!firstPoint) {
                float deltaY = std::abs(screenY - prevY);
                if (deltaY < MAX_JUMP) {
                    coords.push_back({ screenX, screenY });
                }
                else {
                    // Break line at discontinuities to prevent artifacts
                    coords.push_back({ screenX, NAN });
                }
            }
            else {
                coords.push_back({ screenX, screenY });
                firstPoint = false;
            }
            prevY = screenY;
        }
        else {
            // For invalid points in piecewise functions, insert NaN to break the line
            coords.push_back({ screenX, NAN });
        }
    }

    // Clean up expressions
    if (expr) te_free(expr);

    GLuint graphVs, graphFs, graphSp;
    graphVs = compileShader(GL_VERTEX_SHADER, graphVS);
    graphFs = compileShader(GL_FRAGMENT_SHADER, graphFS);
    graphSp = createProgram(graphVs, graphFs);

    GLuint graphVAO, graphVBO;
    glGenVertexArrays(1, &graphVAO);
    glGenBuffers(1, &graphVBO);
    glBindVertexArray(graphVAO);
    glBindBuffer(GL_ARRAY_BUFFER, graphVBO);
    glBufferData(GL_ARRAY_BUFFER, coords.size() * sizeof(std::pair<float, float>), coords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Enable line smoothing for better visuals
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    float animationProgress = 0.0f;
    double lastTime = glfwGetTime();
    bool animationComplete = false;

    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Smoother animation with easing
        if (!animationComplete) {
            animationProgress += deltaTime / 22.0f;
            if (animationProgress >= 1.0f) {
                animationProgress = 1.0f;
                animationComplete = true;
            }
        }

        // Apply easing for smoother progression
        float easedProgress = 1.0f - (1.0f - animationProgress) * (1.0f - animationProgress);

        glClearColor(0.05f, 0.08f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw background quad
        glUseProgram(sp);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Draw grid first (behind everything)
        glUseProgram(gridsp);
        glBindVertexArray(GridVAO);
        glLineWidth(1.0f);
        glDrawArrays(GL_LINES, 0, gridVertices.size() / 2);

        // Draw ticks
        glUseProgram(linesp);
        glBindVertexArray(TickVAO);
        glLineWidth(1.5f);
        glDrawArrays(GL_LINES, 0, tickVertices.size() / 2);

        // Draw axes
        glBindVertexArray(LineVAO);
        glLineWidth(2.0f);
        glDrawArrays(GL_LINES, 0, 4);

        // Draw graph with thicker line
        glUseProgram(graphSp);
        glBindVertexArray(graphVAO);
        glLineWidth(3.5f);

        int pointsToDraw = (int)(coords.size() * easedProgress);
        glDrawArrays(GL_LINE_STRIP, 0, pointsToDraw);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &LineVAO);
    glDeleteBuffers(1, &LineVBO);
    glDeleteVertexArrays(1, &GridVAO);
    glDeleteBuffers(1, &GridVBO);
    glDeleteVertexArrays(1, &TickVAO);
    glDeleteBuffers(1, &TickVBO);
    glDeleteVertexArrays(1, &graphVAO);
    glDeleteBuffers(1, &graphVBO);

    glDeleteProgram(sp);
    glDeleteProgram(linesp);
    glDeleteProgram(gridsp);
    glDeleteProgram(graphSp);

    glfwTerminate();
    return 0;
}