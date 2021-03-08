//===========================================================================//
//                                                                           //
// Copyright(c) ECS 175 (2020)                                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

// In this part the student will setup an OpenGL window
#include "util.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl2.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <string>
#include <vector>

//---------------------------------------------------------------------------------------
// TODO setup callbacks
//---------------------------------------------------------------------------------------
static bool show_gui = false;

static bool
CapturedByGUI()
{
  ImGuiIO& io = ImGui::GetIO();
  return (io.WantCaptureMouse);
}

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
    show_gui = !show_gui;
  }
}

static void
CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
  if (!CapturedByGUI()) {
    int left_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    int right_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    // left click
    if (left_state == GLFW_PRESS) {
    }
    else {
    }

    // right click
    if (right_state == GLFW_PRESS) {
    }
    else {
    }
  }
}

static void
WindowSizeCallback(GLFWwindow* window, int width, int height)
{
}

//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------

void
DrawCall(GLFWwindow* window)
{
  float ratio;
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  ratio = width / (float)height;
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glRotatef((float)glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
  glBegin(GL_TRIANGLES);
  glColor3f(1.f, 0.f, 0.f);
  glVertex3f(-0.6f, -0.4f, 0.f);
  glColor3f(0.f, 1.f, 0.f);
  glVertex3f(0.6f, -0.4f, 0.f);
  glColor3f(0.f, 0.f, 1.f);
  glVertex3f(0.f, 0.6f, 0.f);
  glEnd();
}

//---------------------------------------------------------------------------------------

int
main(const int argc, const char** argv)
{
  printf("In this demo, you can learn how to create\n"
         "an interactive GUI using Dear ImGui and the\n"
         "legacy OpenGL API.\n");

  int width = 800, height = 600;

  // Initialize GLFW
  glfwSetErrorCallback(ErrorCallback);
  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  // Provide Window Hint
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  // OpenGL Setup
  GLFWwindow* window = NULL;
  window = glfwCreateWindow(
    width, height, "ECS 175 Renderer (press 'g' to display GUI)", NULL, NULL);
  if (!window) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }

  // Callback
  glfwSetKeyCallback(window, KeyCallback);
  glfwSetWindowSizeCallback(window, WindowSizeCallback);
  glfwSetCursorPosCallback(window, CursorPositionCallback);

  // Ready
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // Load GLAD symbols
  int err = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0;
  if (err) {
    throw std::runtime_error("Failed to initialize OpenGL loader!");
  }

  // ImGui
  {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark(); // or ImGui::StyleColorsClassic();

    // Initialize Dear ImGui
    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL2_Init();
  }

  // Execute
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw Background
    DrawCall(window);

    // Draw GUI
    {
      // Initialization
      ImGui_ImplOpenGL2_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // - Uncomment below to show ImGui demo window
      if (show_gui)
        ImGui::ShowDemoWindow(&show_gui);

      // Render GUI
      ImGui::Render();
      ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    }
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Exit
  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}
