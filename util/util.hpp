//===========================================================================//
//                                                                           //
// Copyright(c) 2018 Qi Wu (Wilson)                                          //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <glad/glad.h>
// it is necessary to include glad before glfw
#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>
#include <vector>

void
_glCheckError(const char* file, int line, const char* comment);

#ifndef NDEBUG
#define check_error_gl(x) _glCheckError(__FILE__, __LINE__, x)
#else
#define check_error_gl(x) ((void)0)
#endif

#ifndef NDEBUG
#define ASSERT(condition, message)                                                                                \
  do {                                                                                                            \
    if (!(condition)) {                                                                                           \
      std::cerr << "Assertion `" #condition "` failed in " << __FILE__ << " line " << __LINE__ << ": " << message \
                << std::endl;                                                                                     \
      std::terminate();                                                                                           \
    }                                                                                                             \
  } while (false)
#define WARN(condition, message)                                                                                  \
  do {                                                                                                            \
    if (!(condition)) {                                                                                           \
      std::cerr << "Assertion `" #condition "` failed in " << __FILE__ << " line " << __LINE__ << ": " << message \
                << std::endl;                                                                                     \
    }                                                                                                             \
  } while (false)
#else
#define ASSERT(condition, message) \
  do {                             \
  } while (false)
#define WARN(condition, message) \
  do {                           \
  } while (false)
#endif

void
SaveJPG(const std::string& fname, std::vector<uint8_t>& fb, int w, int h);

// Load a texture from a file
GLuint
loadTexture_from_file(const char* imagepath);
GLuint
loadBMP_embeded(const unsigned char* content, size_t bytes);
GLuint
loadBMP_from_file(const char* imagepath);

void
ReadFrame(GLFWwindow* window, std::vector<uint8_t>& buffer, int w, int h);

inline void
ScreenShot(GLFWwindow* window, const std::string& fname)
{
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  std::vector<uint8_t> fb(size_t(width) * size_t(height) * size_t(3));
  ReadFrame(window, fb, width, height);
  SaveJPG(fname, fb, width, height);
}

GLuint
LoadProgram_FromFiles(const char* vshader_fname, const char* fshader_fname);

GLuint
LoadProgram_FromEmbededTexts(const char* vshader_text,
                             long int vshader_size,
                             const char* fshader_text,
                             long int fshader_size);
