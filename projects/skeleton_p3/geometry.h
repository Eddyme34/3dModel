//===========================================================================//
//                                                                           //
// Copyright(c) ECS 175 (2020)                                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#pragma once

#include "util.hpp"

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

struct AbstractGeometry {
  virtual ~AbstractGeometry() = default;

  virtual void
  Clear() = 0;

  virtual void
  Create() = 0;

  virtual void
  Render() = 0;

  glm::mat4
  GetModelMatrix();

  std::string name;
  glm::vec3 center = glm::vec3(0.f);
  glm::vec3 lower, upper;
  float scale = 1.f;
};

inline glm::mat4
AbstractGeometry::GetModelMatrix()
{
  return glm::scale(glm::vec3(scale)) * glm::translate(-center);
}