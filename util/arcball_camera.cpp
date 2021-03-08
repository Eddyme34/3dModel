//===========================================================================//
//                                                                           //
// Copyright(c) ECS 175 (2020)                                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

/*
  The MIT License (MIT)

  Copyright (c) 2016 Will Usher

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "arcball_camera.h"
#include <cmath>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

// Project the point in [-1, 1] screen space onto the arcball sphere
static glm::quat
screen_to_arcball(const glm::vec2& p, ArcballCamera::ArcballType type);

ArcballCamera::ArcballCamera(const glm::vec3& eye,
                             const glm::vec3& center,
                             const glm::vec3& up,
                             ArcballType type)
  : type(type)
{
  const glm::vec3 dir = center - eye;
  glm::vec3 z_axis = glm::normalize(dir); // forward
  glm::vec3 x_axis = glm::normalize(glm::cross(z_axis, glm::normalize(up))); // right vector
  glm::vec3 y_axis = glm::normalize(glm::cross(x_axis, z_axis)); // up vector
  x_axis = glm::normalize(glm::cross(z_axis, y_axis));

  center_translation = glm::inverse(glm::translate(center));
  translation = glm::translate(glm::vec3(0.f, 0.f, -glm::length(dir)));
  rotation = glm::normalize(
    glm::quat_cast(glm::transpose(glm::mat3(x_axis, y_axis, -z_axis))));

  update_camera();
}

void
ArcballCamera::rotate(glm::vec2 prev_mouse, glm::vec2 cur_mouse)
{
  // Clamp mouse positions to stay in NDC
  cur_mouse = glm::clamp(cur_mouse, glm::vec2{ -1, -1 }, glm::vec2{ 1, 1 });
  prev_mouse = glm::clamp(prev_mouse, glm::vec2{ -1, -1 }, glm::vec2{ 1, 1 });

  const glm::quat mouse_cur_ball = screen_to_arcball(cur_mouse, type);
  const glm::quat mouse_prev_ball = screen_to_arcball(prev_mouse, type);

  rotation = mouse_cur_ball * mouse_prev_ball * rotation;
  update_camera();
}

void
ArcballCamera::pan(glm::vec2 mouse_delta)
{
  const float zoom_amount = std::abs(translation[3][2]);
  glm::vec4 motion(mouse_delta.x * zoom_amount, mouse_delta.y * zoom_amount,
                   0.f, 0.f);
  // Find the panning amount in the world space
  motion = inv_camera * motion;

  center_translation = glm::translate(glm::vec3(motion)) * center_translation;
  update_camera();
}

void
ArcballCamera::zoom(const float zoom_amount)
{
  const glm::vec3 motion(0.f, 0.f, zoom_amount);

  translation = glm::translate(motion) * translation;
  update_camera();
}

void
ArcballCamera::arcball_type(ArcballType t)
{
  type = t;
}

const glm::mat4&
ArcballCamera::transform() const
{
  return camera;
}

const glm::mat4&
ArcballCamera::inv_transform() const
{
  return inv_camera;
}

glm::vec3
ArcballCamera::eye() const
{
  return glm::vec3{ inv_camera * glm::vec4{ 0, 0, 0, 1 } };
}

glm::vec3
ArcballCamera::dir() const
{
  return glm::normalize(glm::vec3{ inv_camera * glm::vec4{ 0, 0, -1, 0 } });
}

glm::vec3
ArcballCamera::up() const
{
  return glm::normalize(glm::vec3{ inv_camera * glm::vec4{ 0, 1, 0, 0 } });
}

void
ArcballCamera::update_camera()
{
  camera = translation * glm::mat4_cast(rotation) * center_translation;
  inv_camera = glm::inverse(camera);
}

glm::quat
screen_to_arcball(const glm::vec2& p, ArcballCamera::ArcballType type)
{
  const float dist = glm::dot(p, p);
  if (type == ArcballCamera::SPHERE_AND_HYPERBOLIC_SHEET) {
    // If radius < 0.5 we project the point onto the sphere
    if (dist <= 0.25f) {
      return glm::quat(0.0, p.x, p.y, glm::sqrt(1.f - dist));
    }
    else {
      // otherwise we project the point onto the hyperbolic sheet
      return glm::normalize(glm::quat(0.0, p.x, p.y, 0.5 / glm::sqrt(dist)));
    }
  }
  else {
    // If we're on/in the sphere return the point on it
    if (dist <= 1.f) {
      return glm::quat(0.0, p.x, p.y, glm::sqrt(1.f - dist));
    }
    else {
      // otherwise we project the point onto the sphere
      const glm::vec2 proj = glm::normalize(p);
      return glm::quat(0.0, proj.x, proj.y, 0.f);
    }
  }
}
