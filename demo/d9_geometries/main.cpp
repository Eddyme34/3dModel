//===========================================================================//
//                                                                           //
// Copyright(c) ECS 175 (2020)                                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

// Include standard headers
#include <chrono>
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
#include <glm/gtx/transform.hpp>
using namespace glm;

#include "geometry_cone.h"
#include "geometry_cube.h"
#include "geometry_cylinder.h"
#include "geometry_triangle.h"
AbstractGeometry* objects;

#include "arcball_camera.h"
ArcballCamera camera(vec3(4, 3, -3), vec3(0, 0, 0), vec3(0, 1, 0));

static void
error(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}

static void
keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
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
cursor(GLFWwindow* window, double xpos, double ypos)
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
resize(GLFWwindow* window, int width, int height)
{
}

void
gui(bool* p_open)
{
  // measure frame rate
  static float fps = 0.0f;
  {
    static bool opened = false;
    static int frames = 0;
    static auto start = std::chrono::system_clock::now();
    if (!opened) {
      start = std::chrono::system_clock::now();
      frames = 0;
    }
    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start;
    ++frames;
    if (frames % 10 == 0 || frames == 1) // dont update this too frequently
      fps = frames / elapsed_seconds.count();
    opened = *p_open;
  }

  // draw a fixed GUI window
  const float distance = 10.0f;
  static int corner = 0;
  ImVec2 window_pos = ImVec2((corner & 1) ? ImGui::GetIO().DisplaySize.x - distance : distance,
                             (corner & 2) ? ImGui::GetIO().DisplaySize.y - distance : distance);
  ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
  ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.3f)); // Transparent background
  const auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
  if (ImGui::Begin("Information", NULL, flags)) {

    ImGui::Text("FPS (Hz): %.f\n", fps);

    ImGui::Text("Object Name: %s\n", objects->name.c_str());
    ImGui::Text("       Center: %.f, %.f, %.f\n", objects->center.x, objects->center.y, objects->center.x);
    ImGui::Text("       Lower:  %.f, %.f, %.f\n", objects->lower.x, objects->lower.y, objects->lower.z);
    ImGui::Text("       Uppwe:  %.f, %.f, %.f\n", objects->upper.x, objects->upper.y, objects->upper.z);

    ImGui::End();
  }
  ImGui::PopStyleColor();
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

  glfwSetKeyCallback(window, keyboard);
  glfwSetCursorPosCallback(window, cursor);
  glfwSetWindowSizeCallback(window, resize);

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
  if (argc >= 2) {
    // objects = ReadAsIndexObjects(std::string(argv[1]));
    objects = ReadAsArrayObjects<true>(std::string(argv[1]));
    // objects = ReadAsArrayObjects<false>(std::string(argv[1]));
  }
  else {
    // objects = new CylinderObject(10, 0.1, 10);
    // objects = new ConeObject(10, 1, 2);
    objects = new CubeObject();
  }

  objects->Create();

  // Create and compile our GLSL program from the shaders
  GLuint program_id =
    LoadProgram_FromEmbededTexts((char*)vertexshader, vertexshader_size, (char*)fragmentshader, fragmentshader_size);

  GLuint vertex_array_id;
  glGenVertexArrays(1, &vertex_array_id);
  glBindVertexArray(vertex_array_id);

  // Get a handle for our "MVP" uniform
  GLuint MVP_id = glGetUniformLocation(program_id, "MVP");

  do {

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
    glUseProgram(program_id);

    // Projection
    mat4 projection = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    mat4 MVP = projection * camera.transform() * objects->GetModelMatrix();
    glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &MVP[0][0]);
    objects->Render();

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
  objects->Clear();
  glDeleteProgram(program_id);
  glDeleteVertexArrays(1, &vertex_array_id);

  // Cleanup ImGui
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  delete objects;

  return 0;
}
