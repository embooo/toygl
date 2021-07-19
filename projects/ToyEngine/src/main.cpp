#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main()
{
    int width = 800, height = 800;
    glfwInit();
    
    // Set GLFW to use modern OpenGL with core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create GLFW Window
    GLFWwindow* window = glfwCreateWindow(width, height, "TGLEngine", nullptr, nullptr);
    if(window == nullptr)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();

        return -1;
    }

    // Enable GLFW window
    glfwMakeContextCurrent(window);
    
    // Load all OpenGL functions
    if(!gladLoadGL())
    {
        std::cerr << "Failed to initialize OpenGL context\n";

        return -1;
    }

    // Set the dimensions of the OpenGL viewport in the window
    glViewport(0, 0, width, height);

    // Set the background color
    glClearColor(0.33f, 0.28f, 0.22f, 1.0f);
    // Clear the back buffer and set its color 
    glClear(GL_COLOR_BUFFER_BIT);
    // Swap front buffer with back buffer
    glfwSwapBuffers(window);

    // Run loop
    while(!glfwWindowShouldClose(window))
    {
        // Handle events
        glfwPollEvents();
    }

    // Delete window handle
    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}