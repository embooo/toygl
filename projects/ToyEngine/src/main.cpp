#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

const char *vertexShaderSrc = R"(
    #version 330 core
    layout (location=0) in vec3 aPos;
    out vec3 vertexPos;
    void main()
    {
        vertexPos = aPos;
        gl_Position = vec4(aPos, 1.0);
    }
)";

const char *fragmentShaderSrc = R"(
    #version 330 core
    out vec4 FragColor;
    in vec3 vertexPos;
    void main()
    {
        vec3 color = (vertexPos * 0.5) + 0.5;
        
        FragColor = vec4(color, 1.0f);
    }
)";

GLfloat vertices[3 * 3] = {-0.75f, -0.75f * sqrtf(3) / 3.0f, 0.0f,
                           0.75f,  -0.75f * sqrtf(3) / 3.0f, 0.0f,
                           0.0f,   0.75f * sqrtf(3) / 3.0f,  0.0f};

int main() {
  int width = 800, height = 800;
  glfwInit();

  // Set GLFW to use modern OpenGL with core profile
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create GLFW Window
  GLFWwindow *window =
      glfwCreateWindow(width, height, "TGLEngine", nullptr, nullptr);
  if (window == nullptr) {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();

    return -1;
  }

  // Enable GLFW window
  glfwMakeContextCurrent(window);

  // Load all OpenGL functions
  if (!gladLoadGL()) {
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

  // Create a shader object to represent the vertex shader stage
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  // Load shader source code in shader object and compile it
  glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
  glCompileShader(vertexShader);

  // Check for compile errors
  GLint shaderCompileSuccess = false;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderCompileSuccess);
  if (!shaderCompileSuccess) {
    std::cerr << "Failed to compile shader object !\n";
    return -1;
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
  glCompileShader(fragmentShader);

  // Check for compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &shaderCompileSuccess);
  if (!shaderCompileSuccess) {
    std::cerr << "Failed to compile shader object !\n";
    return -1;
  }

  // Create an empty shader program that will contain the different pipeline
  // stages
  GLuint shaderProgram = glCreateProgram();
  // Specify the shaders that are to be linked together
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  // Link the shaders to create the complete shader program
  glLinkProgram(shaderProgram);

  // Check for linking errors
  GLint programLinkSuccess = false;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &programLinkSuccess);
  if (!programLinkSuccess) {
    std::cerr << "Failed to link shader program.\n";
    return -1;
  }

  // Shader objects are linked against our shader program, we don't need them
  // anymore
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Buffer objects store arrays of unformatted memory in the GPU
  // Vertex Buffer Object store vertex attributes' data
  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Vertex Array Objects store the state (location, format) of the vertex data
  // stored in VBOs
  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Specify the location and format of the vertex data
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // Run loop
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.33f, 0.28f, 0.22f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);

    // Handle events
    glfwPollEvents();
  }

  // Cleanup OpenGL state
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  // Delete window handle
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
