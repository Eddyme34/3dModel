//===========================================================================//
//                                                                           //
// Copyright(c) ECS 175 (2020)                                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#pragma once

#include "geometry.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>

struct CylinderObject : public AbstractGeometry {
  GLuint vertex_buffer_id;
  GLuint normal_buffer_id;
  std::vector<float> vertices;
  std::vector<float> normals;

  int resolution;

  CylinderObject(int resolution, float radius, float height);

  void
  Clear() override;

  void
  Create() override;

  void
  Render() override;
};

inline CylinderObject::CylinderObject(int resolution, float radius, float height) : resolution(resolution)
{
  float angle = 2.f * glm::pi<float>() / resolution;

  // Top
  for (int r = 1; r <= resolution; ++r) {
    float sinPhi0 = glm::sin(angle * (r - 1));
    float sinPhi1 = glm::sin(angle * r);
    float cosPhi0 = glm::cos(angle * (r - 1));
    float cosPhi1 = glm::cos(angle * r);
    vertices.push_back(0);
    vertices.push_back(0);
    vertices.push_back(height);
    vertices.push_back(radius * cosPhi0);
    vertices.push_back(radius * sinPhi0);
    vertices.push_back(height);
    vertices.push_back(radius * cosPhi1);
    vertices.push_back(radius * sinPhi1);
    vertices.push_back(height);
    normals.push_back(0);
    normals.push_back(0);
    normals.push_back(1);
    normals.push_back(0);
    normals.push_back(0);
    normals.push_back(1);
    normals.push_back(0);
    normals.push_back(0);
    normals.push_back(1);
  }

  // Boundary
  for (int r = 1; r <= resolution; ++r) {
    float sinPhi0 = glm::sin(angle * (r - 1));
    float sinPhi1 = glm::sin(angle * r);
    float cosPhi0 = glm::cos(angle * (r - 1));
    float cosPhi1 = glm::cos(angle * r);

    // face 1
    vertices.push_back(radius * cosPhi0);
    vertices.push_back(radius * sinPhi0);
    vertices.push_back(0);

    vertices.push_back(radius * cosPhi1);
    vertices.push_back(radius * sinPhi1);
    vertices.push_back(0);

    vertices.push_back(radius * cosPhi1);
    vertices.push_back(radius * sinPhi1);
    vertices.push_back(height);

    // face 2
    vertices.push_back(radius * cosPhi0);
    vertices.push_back(radius * sinPhi0);
    vertices.push_back(0);

    vertices.push_back(radius * cosPhi1);
    vertices.push_back(radius * sinPhi1);
    vertices.push_back(height);

    vertices.push_back(radius * cosPhi0);
    vertices.push_back(radius * sinPhi0);
    vertices.push_back(height);

    // normal for face 1
    normals.push_back(cosPhi0);
    normals.push_back(sinPhi0);
    normals.push_back(0);

    normals.push_back(cosPhi1);
    normals.push_back(sinPhi1);
    normals.push_back(0);

    normals.push_back(cosPhi1);
    normals.push_back(sinPhi1);
    normals.push_back(0);

    // normal for face 2
    normals.push_back(cosPhi0);
    normals.push_back(sinPhi0);
    normals.push_back(0);

    normals.push_back(cosPhi1);
    normals.push_back(sinPhi1);
    normals.push_back(0);

    normals.push_back(cosPhi0);
    normals.push_back(sinPhi0);
    normals.push_back(0);
  }

  // Bottom
  for (int r = 1; r <= resolution; ++r) {
    float sinPhi0 = glm::sin(angle * (r - 1));
    float sinPhi1 = glm::sin(angle * r);
    float cosPhi0 = glm::cos(angle * (r - 1));
    float cosPhi1 = glm::cos(angle * r);
    vertices.push_back(0);
    vertices.push_back(0);
    vertices.push_back(0);
    vertices.push_back(radius * cosPhi1);
    vertices.push_back(radius * sinPhi1);
    vertices.push_back(0);
    vertices.push_back(radius * cosPhi0);
    vertices.push_back(radius * sinPhi0);
    vertices.push_back(0);
    normals.push_back(0);
    normals.push_back(0);
    normals.push_back(-1);
    normals.push_back(0);
    normals.push_back(0);
    normals.push_back(-1);
    normals.push_back(0);
    normals.push_back(0);
    normals.push_back(-1);
  }

  // Statistics
  name = "cylinder";
  center = glm::vec3(0.f, 0.f, height / 2.f);
  lower = glm::vec3(-radius, -radius, 0);
  upper = glm::vec3(radius, radius, height);
  scale = 1.f / glm::max(height, radius);
}

inline void
CylinderObject::Clear()
{
  glDeleteBuffers(1, &vertex_buffer_id);
  glDeleteBuffers(1, &normal_buffer_id);
}

inline void
CylinderObject::Create()
{
  glGenBuffers(1, &vertex_buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &normal_buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), normals.data(), GL_STATIC_DRAW);
}

inline void
CylinderObject::Render()
{
  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
  glVertexAttribPointer(0, // attribute. No particular reason for 0, but must
                           // match the layout in the shader.
                        3, // size
                        GL_FLOAT, // type
                        GL_FALSE, // normalized?
                        0, // stride
                        (void*)0 // array buffer offset
  );

  // 2nd attribute buffer : colors
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_id);
  glVertexAttribPointer(1, // attribute. No particular reason for 1, but must
                           // match the layout in the shader.
                        3, // size
                        GL_FLOAT, // type
                        GL_FALSE, // normalized?
                        0, // stride
                        (void*)0 // array buffer offset
  );

  // Draw the triangle !
  glDrawArrays(GL_TRIANGLES, 0, 4 * resolution * 3);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}