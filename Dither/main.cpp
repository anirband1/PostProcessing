#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>

#include <lib/shader_s.h>
#define STB_IMAGE_IMPLEMENTATION
#include <lib/stb_image.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
unsigned int screenWidth = 960;
unsigned int screenHeight = 640;

const char *PATH_TO_IMG = "media/forest.jpeg";

bool needRedraw = true;

// FPS tracking variables
double lastTime = 0.0;
int frameCount = 0;
double lastFPSTime = 0.0;

float clamp(float val, float min, float max)
{
    val = val < min ? min : val;
    val = val > max ? max : val;
    return val;
}

void imgToTexID(const char *filename, unsigned int *texture, GLint wrapMode)
{
    glGenTextures(1, texture); // +

    glBindTexture(GL_TEXTURE_2D, *texture); // +

    //* Wrapping method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

    //* Filtering method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // scale down
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // scale up

    //* Mipmaps interpolate method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels; // number of color channels
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

Shader *ditherShader = nullptr;

void showFPS(GLFWwindow *window)
{
    double currentTime = glfwGetTime();
    double delta = currentTime - lastFPSTime;
    frameCount++;

    // Update FPS every second
    if (delta >= 0.5)
    {
        double fps = frameCount / delta;
        double ms = 1000.0 / fps;

        // Update window title with FPS info
        std::stringstream ss;
        ss << "Dither - "
           << std::fixed << std::setprecision(0)
           << fps << " FPS" << std::setprecision(5)
           << "(" << ms << " ms)";
        glfwSetWindowTitle(window, ss.str().c_str());

        // Optional: Print to console
        // std::cout << "FPS: " << fps << " | Frame Time: " << ms << " ms" << std::endl;

        frameCount = 0;
        lastFPSTime = currentTime;
    }
}

void Draw(unsigned int texture)
{
    glClear(GL_COLOR_BUFFER_BIT);

    ditherShader->use();
    ditherShader->setInt("imageTexture1", 0);
    ditherShader->setInt("width", screenWidth);
    ditherShader->setInt("height", screenHeight);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int main()
{
#pragma region 'glfw: initialize and configure'
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

#pragma endregion

#pragma region 'glad'
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
#pragma endregion

#pragma region 'shaders'

    ditherShader = new Shader("dependencies/shaders/dither.vs", "dependencies/shaders/dither.fs");

#pragma endregion

#pragma region arrays and buffers
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    // + vertices replace

    float vertices[] = {
        // vertices        // texture
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // SW
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // NW
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // SE
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f    // NE
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 1, 3};

    // + ---------------------------------------------------

    // int numIndices = sizeof(indices) / sizeof(indices[0]);

    unsigned int VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

#pragma endregion

#pragma region TEXTURES

    unsigned int forestTexture;

    imgToTexID(PATH_TO_IMG, &forestTexture, GL_REPEAT);

    glBindVertexArray(VAO);

#pragma endregion

    lastFPSTime = glfwGetTime();

    // + Render Loop

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.09f, 0.11f, 0.13f, 1.0f);
        processInput(window);

        if (needRedraw)
        {
            std::cout << "redraw" << '\n';
            Draw(forestTexture);
            needRedraw = false;
            glfwSwapBuffers(window);
        }

        showFPS(window);

        // shaderProgram.setInt("kernelSize", kernelSize);

        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    ditherShader->del();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    screenWidth = width / 2;   // retina display ugh
    screenHeight = height / 2; // retina display ugh
    needRedraw = true;
}
