#include <array>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Mesh.h"

std::vector<Vertex> vertices = 
{
    Vertex(glm::vec3(-0.75f, -0.75f, 0.0f)).addColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)),
    Vertex(glm::vec3(0.75f,  -0.75f, 0.0f)).addColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)),
    Vertex(glm::vec3(-0.75f, 0.75f,  0.0f)).addColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)),
    Vertex(glm::vec3(0.75f,  0.75f,  0.0f)).addColor(glm::vec4(0.5f, 0.0f, 0.0f, 1.0f))
};


std::vector<GLuint> indices = {0, 1, 2, /**/ 2, 1, 3};


int main() {

  glfwInit();

  int width = 800, height = 800;

  // Set GLFW to use modern OpenGL with core profile
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create GLFW Window
  GLFWwindow *window = glfwCreateWindow(width, height, "TGLEngine", nullptr, nullptr);
  if (window == nullptr) 
  {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();

    return -1;
  }

  // Enable GLFW window
  glfwMakeContextCurrent(window);

  // Load all OpenGL functions
  if (!gladLoadGL()) 
  {
    std::cerr << "Failed to initialize OpenGL context\n";

    return -1;
  }

  Shader defaultShader("./data/shaders/vertex_default.glsl", 
      "./data/shaders/fragment_default.glsl");

  Shader customShader("./data/shaders/vertex.glsl", 
      "./data/shaders/fragment.glsl");

  Mesh mesh(vertices, indices);
  

  // Run loop
  while (!glfwWindowShouldClose(window)) 
  {
    // Clear the back buffer with a color
    glClearColor(0.33f, 0.28f, 0.22f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw primitives from an index buffer
    mesh.draw(customShader);

    // Swap front buffer with back buffer
    glfwSwapBuffers(window);

    // Handle events
    glfwPollEvents();
  }

  // Delete window handle
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
