//===========================================================================//
//                                                                           //
// Copyright(c) 2018 Qi Wu (Wilson)                                          //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#include "util.hpp"
#include "comm.hpp"

// force imgui to use GLAD
// #define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl2.h>

#include <iostream>
#include <string>
#include <vector>

std::vector<Polygon> polygons;
int pid = 0; // current polygon
static bool show_gui = false;

void
KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  //---------------------------------------------------
  // TODO finish the implementation starting from here
  //---------------------------------------------------
}

void
CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
  //---------------------------------------------------
  // TODO finish the implementation starting from here
  //---------------------------------------------------
}

void
DrawCall(GLFWwindow* window)
{
  //---------------------------------------------------
  // TODO finish the implementation starting from here
  //---------------------------------------------------
}


int
main(const int argc, const char** argv)
{
  if (argc < 2) 
    throw std::runtime_error("missing input file"); 
  
  ReadFile(argv[1]);
  // Compute the Center of Mass
  for (auto& p : polygons) {        
    float cx = 0.f, cy = 0.f;
    for (auto& pt : p.points) {
      cx += pt.x;
      cy += pt.y;
    }
    cx /= (float)p.points.size();
    cy /= (float)p.points.size();
    p.cx = cx;
    p.cy = cy;
  }

  //---------------------------------------------------
  // TODO finish the implementation starting from here
  //---------------------------------------------------

  return EXIT_SUCCESS;
}
