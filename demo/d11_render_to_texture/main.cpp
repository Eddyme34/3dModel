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

#include "arcball_camera.h"
ArcballCamera camera(vec3(4, 3, -3), vec3(0, 0, 0), vec3(0, 1, 0));

#include "geometry_triangle.h"
AbstractGeometry* objects;

// Reference: https://www.khronos.org/opengl/wiki/Framebuffer_Object
struct FrameBufferObject {
  GLuint framebuffer_id = 0;
  // Texture which will contain the RGB output of our shader.
  GLuint color_tex;
  // An optional depth buffer. This enables depth-testing.
  GLuint depth_tex; // depth texture. slower, but you can sample it later in your shader
  GLuint depth_rb; // depth render buffer: faster

  void
  Generate()
  {
    glGenFramebuffers(1, &framebuffer_id);
    glGenTextures(1, &color_tex);
    glGenTextures(1, &depth_tex);
    glGenRenderbuffers(1, &depth_rb);
  }

  void
  Resize(int width, int height, bool enable_depth_texture)
  {
    BindFramebuffer();

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, color_tex);
    {
      // Give an empty image to OpenGL ( the last "0" means "empty" )
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

      // Poor filtering
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_tex, 0);

    // The depth buffer
    if (!enable_depth_texture) {
      glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);
    }
    else {
      glBindTexture(GL_TEXTURE_2D, depth_tex);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_tex, 0);
    }

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      throw std::runtime_error("framebuffer object incomplete");

    // Switch back to the default framebuffer
    UnbindFramebuffer();
  }

  void
  BindFramebuffer()
  {
    // Switch to the framebuffer object
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
  }

  void
  UnbindFramebuffer()
  {
    // Switch back to the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void
  BindColorTexture()
  {
    glBindTexture(GL_TEXTURE_2D, color_tex);
  }

  void
  BindDepthTexture()
  {
    glBindTexture(GL_TEXTURE_2D, depth_tex);
  }

  void
  Clear()
  {
    glDeleteFramebuffers(1, &framebuffer_id);
    glDeleteTextures(1, &color_tex);
    glDeleteTextures(1, &depth_tex);
    glDeleteRenderbuffers(1, &depth_rb);
  }
};

FrameBufferObject fbo;
bool render_depth = false;

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

  ivec2 window_size, framebuffer_size;
  glfwGetWindowSize(window, &window_size.x, &window_size.y);
  glfwGetFramebufferSize(window, &framebuffer_size.x, &framebuffer_size.y);

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

    if (ImGui::Checkbox("Render Depth", &render_depth)) {
      // As we are switching to a different mode, we need to setup the framebuffer object again
      fbo.Resize(framebuffer_size.x, framebuffer_size.y, render_depth);
    }
    
    ImGui::End();
  }
  ImGui::PopStyleColor();
}

void
init()
{
  // Initialise GLFW
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  const char* glsl_version = "#version 150"; // GL 3.3 + GLSL 150
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow(1024, 768, "ECS 175 (press 'g' to display GUI)", NULL, NULL);
  if (window == NULL) {
    glfwTerminate();
    throw std::runtime_error("Failed to open GLFW window. If you have a GPU that is "
                             "not 3.3 compatible, try a lower OpenGL version.");
  }

  glfwSetKeyCallback(window, keyboard);
  glfwSetCursorPosCallback(window, cursor);

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
}

int
main(const int argc, const char** argv)
{
  init();

  ivec2 window_size, framebuffer_size;
  glfwGetWindowSize(window, &window_size.x, &window_size.y);
  glfwGetFramebufferSize(window, &framebuffer_size.x, &framebuffer_size.y);

  // Load Objects
  if (argc >= 2)
    objects = ReadAsArrayObjects(std::string(argv[1]));
  else
    objects = ReadAsArrayObjects("suzanne.obj");
  objects->Create();

  // A shared vertex array
  GLuint vertex_array_id;
  glGenVertexArrays(1, &vertex_array_id);
  glBindVertexArray(vertex_array_id);

  // Create and compile our GLSL program from the shaders
  GLuint program_rtt =
    LoadProgram_FromEmbededTexts((char*)vshader_rtt, vshader_rtt_size, (char*)fshader_rtt, fshader_rtt_size);
  GLuint MVP_id = glGetUniformLocation(program_rtt, "MVP"); // Get a handle for our "MVP" uniform

  // Create and compile our GLSL program from the shaders
  GLuint program_quad =
    LoadProgram_FromEmbededTexts((char*)vshader_quad, vshader_quad_size, (char*)fshader_quad, fshader_quad_size);
  GLuint renderedTexture_id = glGetUniformLocation(program_quad, "renderedTexture");
  GLuint depthTexture_id = glGetUniformLocation(program_quad, "depthTexture");
  GLuint time_id = glGetUniformLocation(program_quad, "time");
  GLuint near_id = glGetUniformLocation(program_quad, "near");
  GLuint far_id = glGetUniformLocation(program_quad, "far");
  GLuint render_depth_id = glGetUniformLocation(program_quad, "render_depth");

  // ---------------------------------------------
  // Render to Texture - specific code begins here
  // ---------------------------------------------
  fbo.Generate();
  fbo.Resize(framebuffer_size.x, framebuffer_size.y, render_depth);

  // The fullscreen quad's FBO
  static const GLfloat g_quad_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
  };

  GLuint quad_vertexbuffer;
  glGenBuffers(1, &quad_vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

  // ---------------------------------------------
  // Rendering loop
  // ---------------------------------------------

  do {

    // Render to our framebuffer
    fbo.BindFramebuffer();
    {
      // Clear the screen
      glViewport(0, 0, framebuffer_size.x, framebuffer_size.y);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);

      // Use the RTT rendering pass
      glUseProgram(program_rtt);

      // Projection
      mat4 projection = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

      // Send our transformation to the currently bound shader,
      // in the "MVP" uniform
      mat4 MVP = projection * camera.transform() * objects->GetModelMatrix();
      glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &MVP[0][0]);
      objects->Render();
    }
    fbo.UnbindFramebuffer();

    // Render to the screen
    glViewport(0, 0, window_size.x, window_size.y);
    {
      glClear(GL_COLOR_BUFFER_BIT);
      glDisable(GL_DEPTH_TEST);

      // Use our shader
      glUseProgram(program_quad);

      // When the type of internal state is boolean, zero integer or
      // floating-point values are converted to FALSE and non-zero
      // values are converted to TRUE
      glUniform1i(render_depth_id, render_depth ? 1 : 0);

      glActiveTexture(GL_TEXTURE0); // Bind our texture in Texture Unit 0
      glUniform1i(renderedTexture_id, 0); // Set our "renderedTexture" sampler to use Texture Unit 0
      fbo.BindColorTexture();

      if (render_depth) {
        glActiveTexture(GL_TEXTURE1); // Bind our texture in Texture Unit 1
        glUniform1i(depthTexture_id, 1); // Set our "depthTexture_id" sampler to use Texture Unit 1
        fbo.BindDepthTexture();
      }

      glUniform1f(time_id, (float)(glfwGetTime() * 10.0f));

      // The near and far value seem to be wrong. You might want to rewrite the 'LinearizeDepth' function
      // defined in the fragment shader.
      glUniform1f(near_id, 0.01f);
      glUniform1f(far_id, 10.f);

      // 1st attribute buffer : vertices
      glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
      glVertexAttribPointer(0, // attribute 0. No particular reason for 0, but must match the layout in the shader.
                            3, // size
                            GL_FLOAT, // type
                            GL_FALSE, // normalized?
                            0, // stride
                            (void*)0 // array buffer offset
      );
      // Draw the triangles !
      glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
      glDisableVertexAttribArray(0);
    }

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

  // Cleanup
  objects->Clear();
  fbo.Clear();
  glDeleteBuffers(1, &quad_vertexbuffer);
  glDeleteVertexArrays(1, &vertex_array_id);
  glDeleteProgram(program_rtt);
  glDeleteProgram(program_quad);

  // Cleanup ImGui
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  delete objects;

  return 0;
}
