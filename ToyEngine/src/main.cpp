#include <array>
#include <iostream>

#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include "Application.h"

//#include "glm/gtc/matrix_transform.hpp"

//std::vector<Vertex> vertices = 
//{
//    Vertex(glm::vec3(-0.75f, -0.75f, 0.0f)).addColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
//    Vertex(glm::vec3(0.75f,  -0.75f, 0.0f)).addColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)),
//    Vertex(glm::vec3(-0.75f, 0.75f,  0.0f)).addColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)),
//    Vertex(glm::vec3(0.75f,  0.75f,  0.0f)).addColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f))
//};
//
//std::vector<Vertex> cubeVertices
//{
//    Vertex(glm::vec3(-0.5f, -0.5f, 0.5f) ).addColor(glm::vec3(-0.5f, -0.5f, 0.5f)), 
//    Vertex(glm::vec3(0.5f, -0.5f, 0.5f)  ).addColor(glm::vec3(0.5f, -0.5f, 0.5f)),
//    Vertex(glm::vec3(0.5f, 0.5f, 0.5f)   ).addColor(glm::vec3(0.5f, 0.5f, 0.5f)),
//    Vertex(glm::vec3(-0.5f, 0.5f, 0.5f)  ).addColor(glm::vec3(-0.5f, 0.5f, 0.5f)),
//    Vertex(glm::vec3(-0.5f, -0.5f, -0.5f)).addColor(glm::vec3(-0.5f, -0.5f, -0.5f)),
//    Vertex(glm::vec3(0.5f, -0.5f, -0.5f) ).addColor(glm::vec3(0.5f, -0.5f, -0.5f)),
//    Vertex(glm::vec3(0.5f, 0.5f, -0.5f)  ).addColor(glm::vec3(0.5f, 0.5f, -0.5f)),
//    Vertex(glm::vec3(-0.5f, 0.5f, -0.5f) ).addColor(glm::vec3(-0.5f, 0.5f, -0.5f))
//};
//
//
//std::vector<GLuint> indices = {0, 1, 2, /**/ 2, 1, 3};
//std::vector<GLuint> cubeIndices = {0, 1, 2,  0, 2, 3, /**/ 4,5,6, 4,6,7, /**/4,0,3, 4,3,7, /**/5,1,2, 5,2,6, /**/ 0,1,5, 0,5,4, /**/ 3,2,6, 3,6,7};


int main() {

  Application app;

  app.printSystemInfo();
  app.run();

  return 0;

  //Shader defaultShader("./data/shaders/vertex_default.glsl", "./data/shaders/fragment_default.glsl");
  //Shader customShader("./data/shaders/vertex.glsl", "./data/shaders/fragment.glsl");

  //Mesh mesh(vertices, indices);
  //Mesh cube(cubeVertices, cubeIndices);

  //Camera camera("DefaultCamera", ViewFrustum(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, -1.0f));

  //// Setup matrices
  //// Init identity matrices
  //glm::mat4 model = glm::identity<glm::mat4>(); // Local space to World space

  //// Discard fragments that fail the depth test
  //glEnable(GL_DEPTH_TEST);

  //// Run loop
  //while (!window.isClosed()) 
  //{
  //  // Specify back buffer's clear color
  //  glClearColor(0.33f, 0.28f, 0.22f, 1.0f);
  //  // Clear depth buffer
  //  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //  // Specify which shader program to use
  //  customShader.use();

  //  // Configure shader uniforms
  //  model = glm::rotate(model, glm::radians(0.1f), glm::vec3(1.0f, 0.0f, 1.0f));

  //  customShader.setMat4("model", model);
  //  customShader.setMat4("view", camera.getViewMat());
  //  customShader.setMat4("projection", camera.getProjectionMat());

  //  // Draw object using current shader 
  //  cube.draw();

  //  window.update();
  //}

  //// Cleanup
  //mesh.release();
  //cube.release();
  //defaultShader.release();
  //customShader.release();

  return 0;
}
