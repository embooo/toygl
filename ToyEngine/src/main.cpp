#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "shader.h"
#include <iostream>
#include <windows.h>

GLfloat vertices[] = {
    -0.75f, -0.75f, 0.0f, // bottom-left
    0.75f,  -0.75f, 0.0f, // bottom-right
    -0.75f, 0.75f,  0.0f, // top-left
    0.75f,  0.75f,  0.0f  // top-right
};

GLuint indices[] = {0, 1, 2, /**/ 2, 1, 3};

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

  const std::string shaderBaseDir = "./data/shaders/";

  Shader mainShader(shaderBaseDir + "vertex_default.glsl",
                    shaderBaseDir + "fragment_default.glsl");
  // Buffer objects store arrays of unformatted memory in the GPU
  // Vertex Buffer Object store vertex attributes' data
  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Vertex Array Objects store the state (configuration, buffer object)
  // of the vertex attributes
  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Element Buffer Objects store indices
  // We can reuse vertices to draw primitives by specifying their indices
  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // Specify the location and format of the vertex data
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // Run loop
  while (!glfwWindowShouldClose(window)) 
  {

    // Specify the shader program used to draw the primitives
    mainShader.use();
    glBindVertexArray(VAO);

    // Clear the back buffer with a color
    glClearColor(0.33f, 0.28f, 0.22f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw primitives from an index buffer
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, /* Offset in EBO */ 0);

    // Swap front buffer with back buffer
    glfwSwapBuffers(window);

    // Handle events
    glfwPollEvents();
  }

  // Cleanup OpenGL state
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  // Delete window handle
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
