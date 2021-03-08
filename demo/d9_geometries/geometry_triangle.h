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

struct TriangleIndexObjects : public AbstractGeometry {
  struct Mesh {
    std::unique_ptr<unsigned int> indices;
    size_t size_indices;
    GLuint vbo_index;
  };
  std::vector<Mesh> meshes;
  std::unique_ptr<float> vertices;
  std::unique_ptr<float> normals;
  size_t size_of_buffer;
  GLuint vbo_vertex, vbo_normal;

  void
  Clear() override;

  void
  Create() override;

  void
  Render() override;
};

struct TriangleArrayObjects : public AbstractGeometry {
  struct Mesh {
    std::unique_ptr<float> vertices;
    std::unique_ptr<float> normals;
    size_t size_triangles;
    GLuint vbo_vertex, vbo_normal;
  };
  std::vector<Mesh> meshes;

  void
  Clear() override;

  void
  Create() override;

  void
  Render() override;
};

TriangleIndexObjects*
ReadAsIndexObjects(const std::string& inputfile);

template<bool ComputePerVertexNormal>
TriangleArrayObjects*
ReadAsArrayObjects(const std::string& inputfile);

template<>
TriangleArrayObjects*
ReadAsArrayObjects<true>(const std::string& inputfile);

template<>
TriangleArrayObjects*
ReadAsArrayObjects<false>(const std::string& inputfile);
