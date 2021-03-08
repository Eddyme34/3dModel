//===========================================================================//
//                                                                           //
// Copyright(c) ECS 175 (2020)                                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#pragma once

#include "geometry.h"

#include "tiny_obj_loader.h"

#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

struct TriangleArrayObjects : public AbstractGeometry {
  struct Mesh {
    std::unique_ptr<float> vertices;
    std::unique_ptr<float> normals;
    std::unique_ptr<float> texcoords;
    size_t size_triangles;
    GLuint vbo_vertex, vbo_normal, vbo_texcoord;
  };
  std::vector<Mesh> meshes;

  void
  Clear() override;

  void
  Create() override;

  void
  Render() override;
};

TriangleArrayObjects*
ReadAsArrayObjects(const std::string& inputfile);
