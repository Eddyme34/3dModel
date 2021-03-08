//===========================================================================//
//                                                                           //
// Copyright(c) 2018 Qi Wu (Wilson)                                          //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#include "comm.hpp"
#include "util.hpp"

#include <fstream>

template<typename T>
T
SafeRead(std::ifstream& is)
{
  T ret;
  is >> ret;
  auto rdstate = is.rdstate();
  if ((rdstate & std::ifstream::eofbit) != 0)
    throw std::runtime_error("End-of-File reached on input operation");
  if ((rdstate & std::ifstream::failbit) != 0)
    throw std::runtime_error("Logical error on i/o operation");
  if ((rdstate & std::ifstream::badbit) != 0)
    throw std::runtime_error("Read/writing error on i/o operation");
  return ret;
}

void
ReadFile(const std::string& input)
{  
  std::ifstream file(input);
  if (!file.is_open())
    throw std::runtime_error("cannot open input: " + input);

  int n = SafeRead<int>(file);
  std::cout << "there are " << n << " polygons" << std::endl;
  
  polygons.resize(n);
  
  for (auto& p : polygons) {
    int m = SafeRead<int>(file);
    for (auto i = 0; i < m; ++i) {
      float x = SafeRead<float>(file), y = SafeRead<float>(file);
      p.points.push_back(Point{x, y});
    }
  }
  file.close();

  std::cout << "done" << std::endl;
}

void
ErrorCallback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}

void MakePix(void* _window, int x, int y, float r, float g, float b)
{
  GLFWwindow* window = (GLFWwindow*)_window;

  float old_point_size;
  glGetFloatv(GL_POINT_SIZE, &old_point_size);

  int width, height;
  glfwGetWindowSize(window, &width, &height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width, 0, height, -1, 1);

  glMatrixMode(GL_MODELVIEW); // clear the modelview matrix
  glLoadIdentity();

  glPointSize(10);
  glBegin(GL_POINTS);
  glColor3f(r, g, b);
  glVertex3f((float)x, (float)y, 0.f);
  glEnd();

  glPointSize(old_point_size);
}
