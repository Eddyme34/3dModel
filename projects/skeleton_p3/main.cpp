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
#include <vector>
#include <fstream>
#include <iostream>
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
//Camera for XY axis
vec3 camera_center(0, 0, 5);
vec3 camera_focus(0, 0, 0);
vec3 camera_up_vector(0, 1, 0);
ArcballCamera camera(camera_center, camera_focus, camera_up_vector);
ArcballCamera camera_external(vec3(0, 0, 10), camera_focus, vec3(0, 1, 0));
//Camera for YZ axis
vec3 camera_center2(5, 0, 0);
vec3 camera_focus2(0, 0, 0);
vec3 camera_up_vector2(0, 1, 0);
ArcballCamera camera2(camera_center2, camera_focus2, camera_up_vector2);
ArcballCamera camera_external2(vec3(0, 0, 10), camera_focus2, vec3(0, 1, 0));
//Camera for XZ axis
vec3 camera_center3(0, 5, 0.1);
vec3 camera_focus3(0, 0, 0);
vec3 camera_up_vector3(0, 1, 0);
ArcballCamera camera3(camera_center3, camera_focus3, camera_up_vector3);
ArcballCamera camera_external3(vec3(0, 0, 10), camera_focus3, vec3(0, 1, 0));

#include "geometry_triangle.h"
TriangleArrayObjects* objects;
std::vector<TriangleArrayObjects*> objectsList;

bool enable_painter_algorithm = true;
static float IA = 0.1;
static float kdx = 0.1;
static float kdy = 0.1;
static float kdz = 0.1;
static float ksx = 0.5;
static float ksy = 0.5;
static float ksz = 0.5;
static int n = 1;
static float kax = 0.5;
static float kay = 0.5;
static float kaz = 0.5;
static float IL = 1.0;
static float xx = 1.0;
static float xy = 1.0;
static float xz = 1.0;
static float fx = 1.0;
static float fy = 1.0;
static float fz = 1.0;
static int radio = 0;
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
void
cursor(GLFWwindow* window, double xpos, double ypos)
{
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
    imgui_enabled = !imgui_enabled;
  }
}


void
WindowSizeCallback(GLFWwindow* window, int width, int height)
{
}
void
init()
{
  // -----------------------------------------------------------
  // For reference only, feel free to make changes
  // -----------------------------------------------------------

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
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // Load GLAD symbols
  int err = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0;
  if (err) {
    throw std::runtime_error("Failed to initialize OpenGL loader!");
  }
  glfwSetKeyCallback(window, KeyCallback);
  glfwSetWindowSizeCallback(window, WindowSizeCallback);
  glfwSetCursorPosCallback(window, cursor);

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
    ImGui_ImplGlfw_InitForOpenGL(
      window, true /* 'true' -> allow imgui to capture keyboard inputs */);
    ImGui_ImplOpenGL3_Init(glsl_version);
  }

  // Dark blue background (avoid using black)
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);
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

    ImGui::SliderFloat("Ambient Intensity", &IA, 0.0, 1.0);
    ImGui::SliderFloat("Light Intensity", &IL, 0.0, 1.0);
    ImGui::SliderFloat("Ambientx", &kax, 0.0, 1.0);
    ImGui::SliderFloat("Ambienty", &kay, 0.0, 1.0);
    ImGui::SliderFloat("Ambientz", &kaz, 0.0, 1.0);
    ImGui::SliderFloat("Diffusex", &kdx, 0.0, 1.0);
    ImGui::SliderFloat("Diffusey", &kdy, 0.0, 1.0);
    ImGui::SliderFloat("Diffusez", &kdz, 0.0, 1.0);
    ImGui::SliderFloat("Specularx", &ksx, 0.0, 1.0);
    ImGui::SliderFloat("Speculary", &ksy, 0.0, 1.0);
    ImGui::SliderFloat("Specularz", &ksz, 0.0, 1.0);
    ImGui::SliderFloat("LightPosx", &xx, -5.0, 5.0);
    ImGui::SliderFloat("LightPosy", &xy, -5.0, 5.0);
    ImGui::SliderFloat("LightPosz", &xz, -5.0, 5.0);
    ImGui::SliderFloat("Viewx", &fx, -5.0, 5.0);
    ImGui::SliderFloat("Viewy", &fy, -5.0, 5.0);
    ImGui::SliderFloat("Viewz", &fz, -5.0, 5.0);
    ImGui::SliderInt("Light Size", &n, 1, 10);
    ImGui::RadioButton("XY", &radio, 0); ImGui::SameLine();
    ImGui::RadioButton("YZ", &radio, 1); ImGui::SameLine();
    ImGui::RadioButton("XZ", &radio, 2);
  ImGui::End();
}
//bubble sort
void
PainterAlgorithm(const TriangleArrayObjects::Mesh& mesh)
{
  // -----------------------------------------------------------
  // TODO: painter's algorithm
  // -----------------------------------------------------------
  float* data_vertex = mesh.vertices.get();
  float* nor_vertex = mesh.normals.get();
  float* uv_vertex = mesh.texcoords.get();
  int index_offset = 3;
  for(int i = 0; i < mesh.size_triangles-1; i++){
    index_offset = 3;
    for(int j = 0; j < mesh.size_triangles - i - 1; j++){
      float min; float min2;
      //sorts by minimum X, Y, or Z depth depending on camera axis
      if(radio == 1){
        min = std::min(data_vertex[3 * (index_offset + 0)], data_vertex[3 * (index_offset + 1)]);
        min = std::min(min, data_vertex[3 * (index_offset + 2) + 2]);
        min2 = std::min(data_vertex[3 * (index_offset - 3 + 0)], data_vertex[3 * (index_offset - 3 + 1)]);
        min2 = std::min(min2, data_vertex[3 * (index_offset - 3 + 2)]);
      }
      if(radio == 2){
        min = std::min(data_vertex[3 * (index_offset + 0) + 1], data_vertex[3 * (index_offset + 1)] + 1);
        min = std::min(min, data_vertex[3 * (index_offset + 2) + 2]);
        min2 = std::min(data_vertex[3 * (index_offset - 3 + 0) + 1], data_vertex[3 * (index_offset - 3 + 1) + 1]);
        min2 = std::min(min2, data_vertex[3 * (index_offset - 3 + 2) + 1]);
      }
      else{
        min = std::min(data_vertex[3 * (index_offset + 0) + 2], data_vertex[3 * (index_offset + 1) + 2]);
        min = std::min(min, data_vertex[3 * (index_offset + 2) + 2]);
        min2 = std::min(data_vertex[3 * (index_offset - 3 + 0) + 2], data_vertex[3 * (index_offset - 3 + 1) + 2]);
        min2 = std::min(min2, data_vertex[3 * (index_offset - 3 + 2) + 2]);
      }
      if(min < min2){
        //sorts vertices
        float temp;
        temp = data_vertex[3 * (index_offset - 3 + 0) + 0];//1
        data_vertex[3 * (index_offset - 3 + 0) + 0] = data_vertex[3 * (index_offset + 0) + 0];
        data_vertex[3 * (index_offset + 0) + 0] = temp;
        temp = data_vertex[3 * (index_offset - 3 + 0) + 1];//2
        data_vertex[3 * (index_offset - 3 + 0) + 1] = data_vertex[3 * (index_offset + 0) + 1];
        data_vertex[3 * (index_offset + 0) + 1] = temp;
        temp = data_vertex[3 * (index_offset - 3 + 0) + 2];//3
        data_vertex[3 * (index_offset - 3 + 0) + 2] = data_vertex[3 * (index_offset + 0) + 2];
        data_vertex[3 * (index_offset + 0) + 2] = temp;
        temp = data_vertex[3 * (index_offset - 3 + 1) + 0];//4
        data_vertex[3 * (index_offset - 3 + 1) + 0] = data_vertex[3 * (index_offset + 1) + 0];
        data_vertex[3 * (index_offset + 1) + 0] = temp;
        temp = data_vertex[3 * (index_offset - 3 + 1) + 1];//5
        data_vertex[3 * (index_offset - 3 + 1) + 1] = data_vertex[3 * (index_offset + 1) + 1];
        data_vertex[3 * (index_offset + 1) + 1] = temp;
        temp = data_vertex[3 * (index_offset - 3 + 1) + 2];//6
        data_vertex[3 * (index_offset - 3 + 1) + 2] = data_vertex[3 * (index_offset + 1) + 2];
        data_vertex[3 * (index_offset + 1) + 2] = temp;
        temp = data_vertex[3 * (index_offset - 3 + 2) + 0];//7
        data_vertex[3 * (index_offset - 3 + 2) + 0] = data_vertex[3 * (index_offset + 2) + 0];
        data_vertex[3 * (index_offset + 2) + 0] = temp;
        temp = data_vertex[3 * (index_offset - 3 + 2) + 1];//8
        data_vertex[3 * (index_offset - 3 + 2) + 1] = data_vertex[3 * (index_offset + 2) + 1];
        data_vertex[3 * (index_offset + 2) + 1] = temp;
        temp = data_vertex[3 * (index_offset - 3 + 2) + 2];//9
        data_vertex[3 * (index_offset - 3 + 2) + 2] = data_vertex[3 * (index_offset + 2) + 2];
        data_vertex[3 * (index_offset + 2) + 2] = temp;
        //sorts normals
        temp = nor_vertex[3 * (index_offset - 3 + 0) + 0];
        nor_vertex[3 * (index_offset - 3 + 0) + 0] = nor_vertex[3 * (index_offset + 0) + 0];
        nor_vertex[3 * (index_offset + 0) + 0] = temp;
        temp = nor_vertex[3 * (index_offset - 3 + 0) + 1];
        nor_vertex[3 * (index_offset - 3 + 0) + 1] = nor_vertex[3 * (index_offset + 0) + 1];
        nor_vertex[3 * (index_offset + 0) + 1] = temp;
        temp = nor_vertex[3 * (index_offset - 3 + 0) + 2];
        nor_vertex[3 * (index_offset - 3 + 0) + 2] = nor_vertex[3 * (index_offset + 0) + 2];
        nor_vertex[3 * (index_offset + 0) + 2] = temp;
        temp = nor_vertex[3 * (index_offset - 3 + 1) + 0];
        nor_vertex[3 * (index_offset - 3 + 1) + 0] = nor_vertex[3 * (index_offset + 1) + 0];
        nor_vertex[3 * (index_offset + 1) + 0] = temp;
        temp = nor_vertex[3 * (index_offset - 3 + 1) + 1];
        nor_vertex[3 * (index_offset - 3 + 1) + 1] = nor_vertex[3 * (index_offset + 1) + 1];
        nor_vertex[3 * (index_offset + 1) + 1] = temp;
        temp = nor_vertex[3 * (index_offset - 3 + 1) + 2];
        nor_vertex[3 * (index_offset - 3 + 1) + 2] = nor_vertex[3 * (index_offset + 1) + 2];
        nor_vertex[3 * (index_offset + 1) + 2] = temp;
        temp = nor_vertex[3 * (index_offset - 3 + 2) + 0];
        nor_vertex[3 * (index_offset - 3 + 2) + 0] = nor_vertex[3 * (index_offset + 2) + 0];
        nor_vertex[3 * (index_offset + 2) + 0] = temp;
        temp = nor_vertex[3 * (index_offset - 3 + 2) + 1];
        nor_vertex[3 * (index_offset - 3 + 2) + 1] = nor_vertex[3 * (index_offset + 2) + 1];
        nor_vertex[3 * (index_offset + 2) + 1] = temp;
        temp = nor_vertex[3 * (index_offset - 3 + 2) + 2];
        nor_vertex[3 * (index_offset - 3 + 2) + 2] = nor_vertex[3 * (index_offset + 2) + 2];
        nor_vertex[3 * (index_offset + 2) + 2] = temp;
        //sorts UV
        temp = uv_vertex[2 * (index_offset - 3 + 0) + 0];
        uv_vertex[2 * (index_offset - 3 + 0) + 0] = uv_vertex[2 * (index_offset + 0) + 0];
        uv_vertex[2 * (index_offset + 0) + 0] = temp;
        temp = uv_vertex[2 * (index_offset - 3 + 0) + 1];
        uv_vertex[2 * (index_offset - 3 + 0) + 1] = uv_vertex[2 * (index_offset + 0) + 1];
        uv_vertex[2 * (index_offset + 0) + 1] = temp;
        temp = uv_vertex[2 * (index_offset - 3 + 1) + 0];
        uv_vertex[2 * (index_offset - 3 + 1) + 0] = uv_vertex[2 * (index_offset + 1) + 0];
        uv_vertex[2 * (index_offset + 1) + 0] = temp;
        temp = uv_vertex[2 * (index_offset - 3 + 1) + 1];
        uv_vertex[2 * (index_offset - 3 + 1) + 1] = uv_vertex[2 * (index_offset + 1) + 1];
        uv_vertex[2 * (index_offset + 1) + 1] = temp;
        temp = uv_vertex[2 * (index_offset - 3 + 2) + 0];
        uv_vertex[2 * (index_offset - 3 + 2) + 0] = uv_vertex[2 * (index_offset + 2) + 0];
        uv_vertex[2 * (index_offset + 2) + 0] = temp;
        temp = uv_vertex[2 * (index_offset - 3 + 2) + 1];
        uv_vertex[2 * (index_offset - 3 + 2) + 1] = uv_vertex[2 * (index_offset + 2) + 1];
        uv_vertex[2 * (index_offset + 2) + 1] = temp;
      }
      index_offset += 3;
    }
  }
    //update buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo_vertex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.size_triangles * 9, mesh.vertices.get(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.size_triangles * 9, mesh.normals.get(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo_texcoord);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.size_triangles * 6, mesh.texcoords.get(), GL_STATIC_DRAW);
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo_vertex);
    glVertexAttribPointer(0, // attribute
                          3, // size
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0, // stride
                          (void*)0 // array buffer offset
    );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo_texcoord);
    glVertexAttribPointer(1, // attribute
                          2, // size
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0, // stride
                          (void*)0 // array buffer offset
    );

    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo_normal);
    glVertexAttribPointer(2, // attribute
                          3, // size
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0, // stride
                          (void*)0 // array buffer offset
    );

    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, mesh.size_triangles * 3);
}

int
main(const int argc, const char** argv)
{
  init();
  ivec2 window_size, framebuffer_size;
  glfwGetWindowSize(window, &window_size.x, &window_size.y);
  glfwGetFramebufferSize(window, &framebuffer_size.x, &framebuffer_size.y);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Create and compile our GLSL program from the shaders
  GLuint program_id = LoadProgram_FromEmbededTexts((char*)vshader, vshader_size, (char*)fshader, fshader_size);
  // Get a handle for our "MVP" uniform
  GLuint MVP_id = glGetUniformLocation(program_id, "MVP");
  GLuint MVP_id11 = glGetUniformLocation(program_id, "MV");
  GLuint MVP_id2 = glGetUniformLocation(program_id, "IA");
  GLuint MVP_id3 = glGetUniformLocation(program_id, "ka");
  GLuint MVP_id4 = glGetUniformLocation(program_id, "IL");
  GLuint MVP_id5 = glGetUniformLocation(program_id, "f");
  GLuint MVP_id7 = glGetUniformLocation(program_id, "kd");
  GLuint MVP_id8 = glGetUniformLocation(program_id, "x");
  GLuint MVP_id9 = glGetUniformLocation(program_id, "ks");
  GLuint MVP_id10 = glGetUniformLocation(program_id, "n");

  GLuint program_quad =
    LoadProgram_FromEmbededTexts((char*)vshader_quad, vshader_quad_size, (char*)fshader_quad, fshader_quad_size);
  GLuint renderedTexture_id = glGetUniformLocation(program_quad, "renderedTexture");
  // Load the texture
  GLuint tex = loadTexture_from_file("uvmap.jpg");

  // Read our .obj file

  //Uncomment this section of code to get the input file

 // std::ifstream inFile;
  //inFile.open(argv[1]);
  //if(inFile.is_open()){
    //std::string nObjects;
   // while(inFile >> nObjects){
    //  TriangleArrayObjects* objects2;
    //  objects2 = ReadAsArrayObjects(nObjects);
    //  objects2->Create();
    //  objectsList.push_back(objects2);
  //}
  //inFile.close();
  //}

//Comment this section out to run the input file part
  objects = ReadAsArrayObjects("suzanne.obj");
  objects->Create();
  //
  fbo.Generate();
  //frame buffer is N/3xM/3 the screen
  fbo.Resize(framebuffer_size.x/3, framebuffer_size.y/3, render_depth);

  // The fullscreen quad's FBO
  static const GLfloat g_quad_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f, 
    1.0f, -1.0f, 0.0f, 
    -1.0f, 1.0f, 0.0f, 
    -1.0f, 1.0f, 0.0f,
     1.0f, -1.0f, 0.0f, 
     1.0f, 1.0f, 0.0f,
  };

  GLuint quad_vertexbuffer;
  glGenBuffers(1, &quad_vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
  do {
    fbo.BindFramebuffer();
    glViewport(0, 0, framebuffer_size.x/3, framebuffer_size.y/3);
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
    glUseProgram(program_id);

    // -----------------------------------------------------------
    // TODO: render your scene
    // -----------------------------------------------------------
    // Send our transformation to the currently bound shader, in the "MVP" uniform
    mat4 P = ortho(-4.f, 4.f, -3.f, 3.f, 0.1f, 100.f);
    mat4 M = objects->GetModelMatrix();
    mat4 V(1.f);
    //GUI interactions determines what camera axis to use
    if(radio == 1){
      V = camera2.transform();
    }
    else if(radio == 2){
      V = camera3.transform();
    }
    else{
      V = camera.transform();
    }
    mat4 MVP = P * V * M;
    mat4 MV = V * M;

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(MVP_id11, 1, GL_FALSE, &MV[0][0]);
    glUniform3f(MVP_id3, kax, kay, kaz);
    glUniform1f(MVP_id2, IA);
    glUniform1f(MVP_id4, IL);
    glUniform3f(MVP_id5, fx, fy, fz);
    glUniform3f(MVP_id7, kdx, kdy, kdz);
    glUniform3f(MVP_id8, xx, xy, xz);
    glUniform3f(MVP_id9, ksx, ksy, ksz);
    glUniform1i(MVP_id10, n);

      // -----------------------------------------------------------
      // NOTE: YOU HAVE TO DISABLE DEPTH TEST FOR P3 !!!!!!!
      // otherwise you get 0 for this part
      // -----------------------------------------------------------
      glDisable(GL_DEPTH_TEST);
      // -----------------------------------------------------------
      // TODO: painter's algorithm
      // -----------------------------------------------------------
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glEnableVertexAttribArray(2);

      //Uncomment the section below to run input code

      //for(auto& objects : objectsList){
        for (auto& m : objects->meshes){
          PainterAlgorithm(m);//render onto screen
      }
      //}
      glDisableVertexAttribArray(0);
      glDisableVertexAttribArray(1);
      glDisableVertexAttribArray(2);

    // -----------------------------------------------------------
    // TODO: post processing (half-toning)
    // -----------------------------------------------------------
    fbo.UnbindFramebuffer();
    // Render to the screen
    glViewport(0, 0, framebuffer_size.x, framebuffer_size.y);
    {
      glClear(GL_COLOR_BUFFER_BIT);
      glDisable(GL_DEPTH_TEST);

      // Use our shader
      glUseProgram(program_quad);


      glActiveTexture(GL_TEXTURE0); // Bind our texture in Texture Unit 0
      glUniform1i(renderedTexture_id, 0); // Set our "renderedTexture" sampler to use Texture Unit 0
      fbo.BindColorTexture();


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
//rasterize based on the current plane.
    // Initialization
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
      // Select which arcball to use
    if (imgui_enabled)
      gui(&imgui_enabled);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  } // Check if the ESC key was pressed or the window was closed
  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

  // Cleanup VBO and shader
  objects->Clear();
  fbo.Clear();
  glDeleteBuffers(1, &quad_vertexbuffer);
  glDeleteProgram(program_id);
  glDeleteTextures(1, &tex);
  glDeleteVertexArrays(1, &vao);

  // Close OpenGL window and terminate GLFW
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();

  delete objects;

  return 0;
}
