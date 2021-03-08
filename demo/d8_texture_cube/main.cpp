//===========================================================================//
//                                                                           //
// Copyright(c) ECS 175 (2020)                                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

// Include standard headers
#include "shaders.h"
#include "util.hpp"
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

int
main(void)
{
  // Initialise GLFW
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow(1024, 768, "ECS 175 - Texture", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr,
            "Failed to open GLFW window. If you have a GPU that is "
            "not 3.3 compatible, try a lower OpenGL version.\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // Load GLAD symbols
  int err = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0;
  if (err) {
    throw std::runtime_error("Failed to initialize OpenGL loader!");
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  GLuint vertex_array_id;
  glGenVertexArrays(1, &vertex_array_id);
  glBindVertexArray(vertex_array_id);

  // Create and compile our GLSL program from the shaders
  GLuint program_id = LoadProgram_FromEmbededTexts((char*)vertexshader, vertexshader_size, (char*)fragmentshader, fragmentshader_size);

  // How you can debug embeded shaders
  // std::cout << vertexshader << std::endl;
  // std::cout << fragmentshader << std::endl;

  // Get a handle for our "MVP" uniform
  GLuint matrix_id = glGetUniformLocation(program_id, "MVP");
  glm::mat4 matrix(1.086396, -0.993682, -0.687368, -0.685994, // x0, y0, z0, w0
                   0.000000, 2.070171, -0.515526, -0.514496, // x1, y1, z1, w1
                   -1.448528, -0.745262, -0.515526, -0.514496, // x2, y2, z2, w2
                   0.000000, 0.000000, 5.642426, 5.830953); // x3, y3, z3, w3

  // Get a handle for our "mytextureSampler" uniform
  GLuint texture_id = glGetUniformLocation(program_id, "mytextureSampler");
  // Load the texture using any two methods
  // GLuint texture = loadBMP_from_file("uvtemplate.bmp");
  GLuint texture = loadTexture_from_file("uvtemplate.bmp");

  // This feature is only available since OpenGL 4.6
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16);

  // Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive
  // vertices give a triangle. A cube has 6 faces with 2 triangles each, so this
  // makes 6*2=12 triangles, and 12*3 vertices
  static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
    1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,
    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  -1.0f,
    1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f, -1.0f, 1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,
    -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f, 1.0f,  -1.0f, 1.0f
  };

  // Two UV coordinatesfor each vertex. They were created with Blender.
  static const GLfloat g_uvcoord_buffer_data[] = {
    0.000059f, 0.000004f, 0.000103f, 0.336048f, 0.335973f, 0.335903f, 1.000023f, 0.000013f, 0.667979f,
    0.335851f, 0.999958f, 0.336064f, 0.667979f, 0.335851f, 0.336024f, 0.671877f, 0.667969f, 0.671889f,
    1.000023f, 0.000013f, 0.668104f, 0.000013f, 0.667979f, 0.335851f, 0.000059f, 0.000004f, 0.335973f,
    0.335903f, 0.336098f, 0.000071f, 0.667979f, 0.335851f, 0.335973f, 0.335903f, 0.336024f, 0.671877f,
    1.000004f, 0.671847f, 0.999958f, 0.336064f, 0.667979f, 0.335851f, 0.668104f, 0.000013f, 0.335973f,
    0.335903f, 0.667979f, 0.335851f, 0.335973f, 0.335903f, 0.668104f, 0.000013f, 0.336098f, 0.000071f,
    0.000103f, 0.336048f, 0.000004f, 0.671870f, 0.336024f, 0.671877f, 0.000103f, 0.336048f, 0.336024f,
    0.671877f, 0.335973f, 0.335903f, 0.667969f, 0.671889f, 1.000004f, 0.671847f, 0.667979f, 0.335851f
  };

  GLuint vertex_buffer_id;
  glGenBuffers(1, &vertex_buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

  GLuint uv_buffer_id;
  glGenBuffers(1, &uv_buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_uvcoord_buffer_data), g_uvcoord_buffer_data, GL_STATIC_DRAW);

  do {

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
    glUseProgram(program_id);

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &matrix[0][0]);

    // Bind our texture in texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Set our "mytextureSampler" sampler to use texture Unit 0
    glUniform1i(texture_id, 0);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glVertexAttribPointer(0, // attribute. No particular reason for 0, but must
                             // match the layout in the shader.
                          3, // size
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0, // stride
                          (void*)0 // array buffer offset
    );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_id);
    glVertexAttribPointer(1, // attribute. No particular reason for 1, but must
                             // match the layout in the shader.
                          2, // size : U+V => 2
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0, // stride
                          (void*)0 // array buffer offset
    );

    // Draw the triangle !
    // 12*3 indices starting at 0 -> 12 triangles
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  }
  // Check if the ESC key was pressed or the window was closed
  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

  // Cleanup VBO and shader
  glDeleteBuffers(1, &vertex_buffer_id);
  glDeleteBuffers(1, &uv_buffer_id);
  glDeleteProgram(program_id);
  glDeleteTextures(1, &texture);
  glDeleteVertexArrays(1, &vertex_array_id);

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}
