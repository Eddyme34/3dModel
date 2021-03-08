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
#include <glm/gtx/string_cast.hpp>
using namespace glm;

enum struct DrawMode {
  DRAW_LINES = 0,
  DRAW_TRIANGLE_FILLED,
  DRAW_TRIANGLE_FRAME,
  SIZE
} draw_mode;

static void
keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    // close window
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  else if (key == GLFW_KEY_N && action == GLFW_PRESS) {
    // ugly hack, never mind ...
    draw_mode = (DrawMode)(((int)draw_mode + 1) % (int)DrawMode::SIZE);
  }
}

int
main(void)
{
  // Initialise GLFW
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    getchar();
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  // We don't want the old OpenGL
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow(1024, 768, "ECS 175 - (Press 'n'...)", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr,
            "Failed to open GLFW window. If you have a GPU that is "
            "not 3.3 compatible, try a lower OpenGL version.\n");
    getchar();
    glfwTerminate();
    return -1;
  }

  glfwSetKeyCallback(window, keyboard);

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

  GLuint vertex_array_id;
  glGenVertexArrays(1, &vertex_array_id);
  glBindVertexArray(vertex_array_id);

  // Create and compile our GLSL program from the shaders
  GLuint program_id =
    LoadProgram_FromEmbededTexts((char*)vertexshader, vertexshader_size, (char*)fragmentshader, fragmentshader_size);

  // Get a handle for our "MVP" uniform
  GLuint matrix_id = glGetUniformLocation(program_id, "Matrix");
  glm::mat4 matrix(1.086396, -0.993682, -0.687368, -0.685994, // x0, y0, z0, w0
                   0.000000, 2.070171, -0.515526, -0.514496, // x1, y1, z1, w1
                   -1.448528, -0.745262, -0.515526, -0.514496, // x2, y2, z2, w2
                   0.000000, 0.000000, 5.642426, 5.830953); // x3, y3, z3, w3

  std::cout << glm::to_string(matrix) << std::endl;

  static const GLfloat g_vertex_buffer_data_triangle[] = {
    -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
  };

  static const GLfloat g_vertex_buffer_data_lines[] = {
    // edge 0
    -1.0f, -1.0f, 0.0f, // v0
    1.0f, -1.0f, 0.0f, //  v1
    // edge 1
    1.0f, -1.0f, 0.0f, //  v1
    0.0f, 1.0f, 0.0f, //   v2
    // edge 2
    -1.0f, -1.0f, 0.0f, // v0
    0.0f, 1.0f, 0.0f, //   v2
  };

  GLuint vertex_buffer_id_triangle;
  glGenBuffers(1, &vertex_buffer_id_triangle);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_triangle);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_triangle),
               g_vertex_buffer_data_triangle, GL_STATIC_DRAW);

  GLuint vertex_buffer_id_lines;
  glGenBuffers(1, &vertex_buffer_id_lines);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_lines);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_lines),
               g_vertex_buffer_data_lines, GL_STATIC_DRAW);

  do {

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT);

    // Use our shader
    glUseProgram(program_id);

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &matrix[0][0]);

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    if (draw_mode == DrawMode::DRAW_LINES) {
      glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_lines);
    }
    else {
      glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_triangle);
    }
    glVertexAttribPointer(0, // attribute. No particular reason for 0, but must
                             // match the layout in the shader.
                          3, // size
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0, // stride
                          (void*)0 // array buffer offset
    );

    // Draw !
    if (draw_mode == DrawMode::DRAW_LINES) {
      // 6 indices starting at 0 -> 3 lines
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glDrawArrays(GL_LINES, 0, 6);
    }
    else if (draw_mode == DrawMode::DRAW_TRIANGLE_FILLED) {
      // 3 indices starting at 0 -> 1 triangle
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    else {
      // 3 indices starting at 0 -> 1 triangle
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    glDisableVertexAttribArray(0);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  }
  // Check if the ESC key was pressed or the window was closed
  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0);

  // Cleanup VBO and shader
  glDeleteBuffers(1, &vertex_buffer_id_triangle);
  glDeleteBuffers(1, &vertex_buffer_id_lines);
  glDeleteProgram(program_id);
  glDeleteVertexArrays(1, &vertex_array_id);

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}
