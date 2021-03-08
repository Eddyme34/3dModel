//===========================================================================//
//                                                                           //
// Copyright(c) ECS 175 (2020)                                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

// Include standard headers
#include <stdio.h>
#include <stdlib.h>

#include "shaders.h"
#include "util.hpp"
GLFWwindow* window;

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
bool imgui_enabled = true;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "geometry_cone.h"
#include "geometry_cube.h"
#include "geometry_cylinder.h"

#include "arcball_camera.h"
// Camera is at (4,3,-3), in World Space
vec3 camera_center(0, 0, 3);
// and looks at the origin
vec3 camera_focus(0, 0, 0);
// Head is up (set to 0,-1,0 to look upside-down)
vec3 camera_up_vector(0, 1, 0);
// Camera
ArcballCamera camera(camera_center, camera_focus, camera_up_vector);
ArcballCamera camera_external(vec3(0, 0, 10), camera_focus, vec3(0, 1, 0));
bool use_ortho = false;

GLuint MVP_id;
mat4 MVP;

struct AxisGlyph {
  const float length = 15;
  ConeObject cone = ConeObject(10, 2, 4);
  CylinderObject tube = CylinderObject(10, 0.5f, length);

  AxisGlyph()
  {
    cone.Create();
    tube.Create();
  }

  void
  Render(const mat4& MVP_world)
  {
    mat4 MVP, S = scale(vec3(0.1f));

    MVP = MVP_world * S * translate(vec3(0, 0, length));
    glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &MVP[0][0]);
    cone.Render();

    MVP = MVP_world * S;
    glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &MVP[0][0]);
    tube.Render();
  }
};

struct XYZGlyph {
  AxisGlyph x, y, z;

  void
  Render(const mat4& MVP)
  {
    x.Render(MVP * rotate(0.5f * pi<float>(), vec3(0, 1, 0)));
    y.Render(MVP * rotate(-0.5f * pi<float>(), vec3(1, 0, 0)));
    z.Render(MVP);
  }
};

static void
ErrorCallback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}

static void
KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    // close window
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  else if (key == GLFW_KEY_G && action == GLFW_PRESS) {
    imgui_enabled = !imgui_enabled;
  }
}

static void
CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
  ImGuiIO& io = ImGui::GetIO();
  if (!io.WantCaptureMouse) {
    int left_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    int right_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    static vec2 prev_cursor;
    vec2 cursor((xpos / width - 0.5f) * 2.f, (0.5f - ypos / height) * 2.f);

    // right click -> zoom
    if (right_state == GLFW_PRESS || right_state == GLFW_REPEAT) {
      camera.zoom(cursor.y - prev_cursor.y);
    }

    // left click -> rotate
    if (left_state == GLFW_PRESS || left_state == GLFW_REPEAT) {
      camera.rotate(prev_cursor, cursor);
    }

    prev_cursor = cursor;
  }
}

void
WindowSizeCallback(GLFWwindow* window, int width, int height)
{
}

void
gui(bool* p_open)
{
  ImGuiWindowFlags window_flags = 0;
  if (!ImGui::Begin("Graphical User Interface", p_open, window_flags)) {
    ImGui::End();
    return;
  }

  // Select which arcball to use
  const char* arcball_type_items[] = {
    "Sphere and Hyperbolic Sheet",
    "Simple Sphere",
  };
  static int arcball_type_selection = 0;
  if (ImGui::ListBox(
        "Arcball Type", &arcball_type_selection, arcball_type_items, IM_ARRAYSIZE(arcball_type_items), 2)) {
    if (arcball_type_selection == 0) {
      camera.arcball_type(ArcballCamera::SPHERE_AND_HYPERBOLIC_SHEET);
    }
    else {
      camera.arcball_type(ArcballCamera::SPHERE);
    }
  };

  ImGui::Checkbox("Orthographic Projection", &use_ortho);

  ImGui::End();
}

int
main(const int argc, const char** argv)
{
  // Initialise GLFW
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    getchar();
    return -1;
  }

  const char* glsl_version = "#version 150"; // GL 3.3 + GLSL 150
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow(1024, 768, "ECS 175 (press 'g' to display GUI)", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr,
            "Failed to open GLFW window. If you have a GPU that is "
            "not 3.3 compatible, try a lower OpenGL version.\n");
    getchar();
    glfwTerminate();
    return -1;
  }

  glfwSetKeyCallback(window, KeyCallback);
  glfwSetWindowSizeCallback(window, WindowSizeCallback);
  glfwSetCursorPosCallback(window, CursorPositionCallback);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // Load GLAD symbols
  int err = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0;
  if (err) {
    throw std::runtime_error("Failed to initialize OpenGL loader!");
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  // ImGui
  {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // or ImGui::StyleColorsClassic();

    // Initialize Dear ImGui
    ImGui_ImplGlfw_InitForOpenGL(window, true /* 'true' -> allow imgui to capture keyboard inputs */);
    ImGui_ImplOpenGL3_Init(glsl_version);
  }

  // Dark blue background (avoid using black)
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  // Load Objects
  CubeObject obj_target;
  obj_target.Create();
  ConeObject obj_camera(4, 1, 2);
  obj_camera.Create();

  XYZGlyph axes;

  // Create and compile our GLSL program from the shaders
  GLuint program_id =
    LoadProgram_FromEmbededTexts((char*)vertexshader, vertexshader_size, (char*)fragmentshader, fragmentshader_size);

  GLuint vertex_array_id;
  glGenVertexArrays(1, &vertex_array_id);
  glBindVertexArray(vertex_array_id);

  // Get a handle for our "MVP" uniform
  MVP_id = glGetUniformLocation(program_id, "MVP");

  do {

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
    glUseProgram(program_id);

    // Projection matrix
    mat4 projection =
      use_ortho ? ortho(-4.f, 4.f, -3.f, 3.f, 0.1f, 100.f) : perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    mat4 M(1.f), V(1.f);

    glViewport(0, 0, width, height);

    // Normal View
    {
      M = obj_target.GetModelMatrix();
      V = camera.transform();
      MVP = projection * V * M;
      glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &MVP[0][0]);
      obj_target.Render();
    }

    glViewport(0, 0, 200, height / (float)width * 200);
    MVP = ortho(-4.f, 4.f, -3.f, 3.f, 0.1f, 100.f) * camera.transform() * scale(vec3(0.5f));
    axes.Render(MVP);

    // Draw GUI
    {
      // Initialization
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // - Uncomment below to show ImGui demo window
      if (imgui_enabled)
        gui(&imgui_enabled);

      // Render GUI
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  }
  // Check if the ESC key was pressed or the window was closed
  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

  // Cleanup VBO and shader
  obj_target.Clear();
  obj_camera.Clear();
  glDeleteProgram(program_id);
  glDeleteVertexArrays(1, &vertex_array_id);

  // Cleanup ImGui
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}
