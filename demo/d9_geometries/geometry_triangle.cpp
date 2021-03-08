//===========================================================================//
//                                                                           //
// Copyright(c) ECS 175 (2020)                                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#include "geometry_triangle.h"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

#include <cassert>
#include <limits>
#include <numeric>

TriangleIndexObjects*
ReadAsIndexObjects(const std::string& inputfile)
{
  auto char_id = inputfile.find_last_of('/');
  auto directory = inputfile.substr(0, char_id);
  auto filename = inputfile.substr(char_id + 1, inputfile.size() - char_id - 1);

  TriangleIndexObjects* output = new TriangleIndexObjects();

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn;
  std::string err;

  bool ret = tinyobj::LoadObj(
    &attrib, &shapes, &materials, &warn, &err, inputfile.c_str(), directory.c_str(), true /* force triangulate */);

  if (!warn.empty()) {
    std::cout << warn << std::endl;
  }

  if (!err.empty()) {
    std::cerr << err << std::endl;
  }

  if (!ret) {
    throw std::runtime_error("failed to read file " + inputfile);
  }

  size_t total_num_of_vertices = 0;
  glm::vec3 total_lower_bound(+std::numeric_limits<float>::max());
  glm::vec3 total_upper_bound(-std::numeric_limits<float>::max());

  // Copy vertices
  output->size_of_buffer = attrib.vertices.size();
  output->vertices.reset(new float[output->size_of_buffer]);
  std::copy(&(attrib.vertices[0]), &(attrib.vertices[0]) + attrib.vertices.size(), output->vertices.get());
  // Handle normals
  output->normals.reset(new float[output->size_of_buffer]());
  auto* data_n = output->normals.get();
  bool computing_normals = false;
  if (attrib.normals.empty()) {
    computing_normals = true;
  }
  std::cout << "size of buffer = " << output->size_of_buffer << std::endl;
  std::cout << "# vertices = " << attrib.vertices.size() << std::endl;
  std::cout << "# normals = " << attrib.normals.size() << std::endl;

  // Loop over shapes
  for (size_t s = 0; s < shapes.size(); s++) {

    // Compute size
    const size_t num_of_vertices = std::accumulate(shapes[s].mesh.num_face_vertices.begin(),
                                                   shapes[s].mesh.num_face_vertices.end(),
                                                   size_t(0),
                                                   [](size_t init, uint8_t value) { return init + value; });
    total_num_of_vertices += num_of_vertices;

    // Create a mesh
    output->meshes.emplace_back();
    auto& g = output->meshes.back();
    g.size_indices = num_of_vertices;
    g.indices.reset(new unsigned int[g.size_indices]);
    auto* data_indices = g.indices.get();

    // Loop over faces (polygon)
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      int fv = shapes[s].mesh.num_face_vertices[f];
      assert(fv == 3);

      // Access to vertex
      tinyobj::index_t idx0 = shapes[s].mesh.indices[index_offset + 0];
      tinyobj::index_t idx1 = shapes[s].mesh.indices[index_offset + 1];
      tinyobj::index_t idx2 = shapes[s].mesh.indices[index_offset + 2];

      glm::vec3 v0 = glm::vec3(attrib.vertices[3 * idx0.vertex_index + 0],
                               attrib.vertices[3 * idx0.vertex_index + 1],
                               attrib.vertices[3 * idx0.vertex_index + 2]);
      glm::vec3 v1 = glm::vec3(attrib.vertices[3 * idx1.vertex_index + 0],
                               attrib.vertices[3 * idx1.vertex_index + 1],
                               attrib.vertices[3 * idx1.vertex_index + 2]);
      glm::vec3 v2 = glm::vec3(attrib.vertices[3 * idx2.vertex_index + 0],
                               attrib.vertices[3 * idx2.vertex_index + 1],
                               attrib.vertices[3 * idx2.vertex_index + 2]);

      data_indices[index_offset + 0] = idx0.vertex_index;
      data_indices[index_offset + 1] = idx1.vertex_index;
      data_indices[index_offset + 2] = idx2.vertex_index;

      if (computing_normals) {
        glm::vec3 e10 = v1 - v0;
        glm::vec3 e20 = v2 - v0;
        glm::vec3 N = glm::cross(e10, e20);
        data_n[3 * idx0.vertex_index + 0] += N.x;
        data_n[3 * idx0.vertex_index + 1] += N.y;
        data_n[3 * idx0.vertex_index + 2] += N.z;
        data_n[3 * idx1.vertex_index + 0] += N.x;
        data_n[3 * idx1.vertex_index + 1] += N.y;
        data_n[3 * idx1.vertex_index + 2] += N.z;
        data_n[3 * idx2.vertex_index + 0] += N.x;
        data_n[3 * idx2.vertex_index + 1] += N.y;
        data_n[3 * idx2.vertex_index + 2] += N.z;
      }
      else {
        assert(idx0.normal_index != -1);
        data_n[3 * idx0.vertex_index + 0] = attrib.normals[3 * idx0.normal_index + 0];
        data_n[3 * idx0.vertex_index + 1] = attrib.normals[3 * idx0.normal_index + 1];
        data_n[3 * idx0.vertex_index + 2] = attrib.normals[3 * idx0.normal_index + 2];
        assert(idx1.normal_index != -1);
        data_n[3 * idx1.vertex_index + 0] = attrib.normals[3 * idx1.normal_index + 0];
        data_n[3 * idx1.vertex_index + 1] = attrib.normals[3 * idx1.normal_index + 1];
        data_n[3 * idx1.vertex_index + 2] = attrib.normals[3 * idx1.normal_index + 2];
        assert(idx2.normal_index != -1);
        data_n[3 * idx2.vertex_index + 0] = attrib.normals[3 * idx2.normal_index + 0];
        data_n[3 * idx2.vertex_index + 1] = attrib.normals[3 * idx2.normal_index + 1];
        data_n[3 * idx2.vertex_index + 2] = attrib.normals[3 * idx2.normal_index + 2];
      }

      // Statistics
      output->center += v0 + v1 + v2;
      total_upper_bound = glm::max(total_upper_bound, v0);
      total_upper_bound = glm::max(total_upper_bound, v1);
      total_upper_bound = glm::max(total_upper_bound, v2);
      total_lower_bound = glm::min(total_lower_bound, v0);
      total_lower_bound = glm::min(total_lower_bound, v1);
      total_lower_bound = glm::min(total_lower_bound, v2);

      // Update
      index_offset += fv; // == 3
    }
  }

  // Normalize normals
  for (size_t i = 0; i < output->size_of_buffer / 3; i++) {
    glm::vec3 n = glm::normalize(glm::vec3(data_n[3 * i + 0], data_n[3 * i + 1], data_n[3 * i + 2]));
    data_n[3 * i + 0] = n.x;
    data_n[3 * i + 1] = n.y;
    data_n[3 * i + 2] = n.z;
  }

  // Information
  output->name = filename;
  output->center /= total_num_of_vertices; // Center of mass
  output->lower = total_lower_bound;
  output->upper = total_upper_bound;
  glm::vec3 s = total_upper_bound - total_lower_bound;
  output->scale = 4.f / glm::max(s.x, glm::max(s.y, s.z));

  return output;
}

template<>
TriangleArrayObjects*
ReadAsArrayObjects<true>(const std::string& inputfile)
{
  auto char_id = inputfile.find_last_of('/');
  auto directory = inputfile.substr(0, char_id);
  auto filename = inputfile.substr(char_id + 1, inputfile.size() - char_id - 1);

  auto* geometries = new TriangleArrayObjects();

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn;
  std::string err;

  bool ret = tinyobj::LoadObj(
    &attrib, &shapes, &materials, &warn, &err, inputfile.c_str(), directory.c_str(), true /* force triangulate */);

  if (!warn.empty()) {
    std::cout << warn << std::endl;
  }

  if (!err.empty()) {
    std::cerr << err << std::endl;
  }

  if (!ret) {
    throw std::runtime_error("failed to read file " + inputfile);
  }

  size_t total_num_of_vertices = 0;
  glm::vec3 total_lower_bound(+std::numeric_limits<float>::max());
  glm::vec3 total_upper_bound(-std::numeric_limits<float>::max());

  bool computing_normals = false;
  if (attrib.normals.empty()) {
    computing_normals = true;
    // compute per vertex normal
    attrib.normals = std::vector<tinyobj::real_t>(attrib.vertices.size(), 0.f);
  }

  // Loop over shapes
  for (size_t s = 0; s < shapes.size(); s++) {

    // Compute size
    const size_t num_of_vertices = std::accumulate(shapes[s].mesh.num_face_vertices.begin(),
                                                   shapes[s].mesh.num_face_vertices.end(),
                                                   size_t(0),
                                                   [](size_t init, uint8_t value) { return init + value; });
    total_num_of_vertices += num_of_vertices;

    // Create a geometry
    geometries->meshes.emplace_back();
    auto& g = geometries->meshes.back();
    g.size_triangles = num_of_vertices / 3;
    g.vertices.reset(new float[g.size_triangles * 9]);
    g.normals.reset(new float[g.size_triangles * 9]() /* zero initialize */);
    auto* data_v = g.vertices.get();
    auto* data_n = g.normals.get();

    // Loop over faces (polygon)
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {

      // Verify
      int fv = shapes[s].mesh.num_face_vertices[f];
      if (fv != 3)
        throw std::runtime_error("non-triangle faces are not supported");

      // Access to vertex
      tinyobj::index_t& idx0 = shapes[s].mesh.indices[index_offset + 0];
      tinyobj::index_t& idx1 = shapes[s].mesh.indices[index_offset + 1];
      tinyobj::index_t& idx2 = shapes[s].mesh.indices[index_offset + 2];
      glm::vec3 v0 = glm::vec3(attrib.vertices[3 * idx0.vertex_index + 0],
                               attrib.vertices[3 * idx0.vertex_index + 1],
                               attrib.vertices[3 * idx0.vertex_index + 2]);
      glm::vec3 v1 = glm::vec3(attrib.vertices[3 * idx1.vertex_index + 0],
                               attrib.vertices[3 * idx1.vertex_index + 1],
                               attrib.vertices[3 * idx1.vertex_index + 2]);
      glm::vec3 v2 = glm::vec3(attrib.vertices[3 * idx2.vertex_index + 0],
                               attrib.vertices[3 * idx2.vertex_index + 1],
                               attrib.vertices[3 * idx2.vertex_index + 2]);

      // Fill the vertex buffer
      data_v[3 * (index_offset + 0) + 0] = v0.x;
      data_v[3 * (index_offset + 0) + 1] = v0.y;
      data_v[3 * (index_offset + 0) + 2] = v0.z;
      data_v[3 * (index_offset + 1) + 0] = v1.x;
      data_v[3 * (index_offset + 1) + 1] = v1.y;
      data_v[3 * (index_offset + 1) + 2] = v1.z;
      data_v[3 * (index_offset + 2) + 0] = v2.x;
      data_v[3 * (index_offset + 2) + 1] = v2.y;
      data_v[3 * (index_offset + 2) + 2] = v2.z;

      // Fill the normal buffer
      if (!computing_normals) {
        assert(idx0.normal_index != -1);
        data_n[3 * (index_offset + 0) + 0] = attrib.normals[3 * idx0.normal_index + 0];
        data_n[3 * (index_offset + 0) + 1] = attrib.normals[3 * idx0.normal_index + 1];
        data_n[3 * (index_offset + 0) + 2] = attrib.normals[3 * idx0.normal_index + 2];
        assert(idx1.normal_index != -1);
        data_n[3 * (index_offset + 1) + 0] = attrib.normals[3 * idx1.normal_index + 0];
        data_n[3 * (index_offset + 1) + 1] = attrib.normals[3 * idx1.normal_index + 1];
        data_n[3 * (index_offset + 1) + 2] = attrib.normals[3 * idx1.normal_index + 2];
        assert(idx2.normal_index != -1);
        data_n[3 * (index_offset + 2) + 0] = attrib.normals[3 * idx2.normal_index + 0];
        data_n[3 * (index_offset + 2) + 1] = attrib.normals[3 * idx2.normal_index + 1];
        data_n[3 * (index_offset + 2) + 2] = attrib.normals[3 * idx2.normal_index + 2];
      }
      else {
        // Compute Normal (flat shading)
        glm::vec3 e10 = v1 - v0;
        glm::vec3 e20 = v2 - v0;
        glm::vec3 N = glm::cross(e10, e20);
        idx0.normal_index = idx0.vertex_index;
        attrib.normals[3 * idx0.normal_index + 0] += N.x;
        attrib.normals[3 * idx0.normal_index + 1] += N.y;
        attrib.normals[3 * idx0.normal_index + 2] += N.z;
        idx1.normal_index = idx1.vertex_index;
        attrib.normals[3 * idx1.normal_index + 0] += N.x;
        attrib.normals[3 * idx1.normal_index + 1] += N.y;
        attrib.normals[3 * idx1.normal_index + 2] += N.z;
        idx2.normal_index = idx2.vertex_index;
        attrib.normals[3 * idx2.normal_index + 0] += N.x;
        attrib.normals[3 * idx2.normal_index + 1] += N.y;
        attrib.normals[3 * idx2.normal_index + 2] += N.z;
      }

      // Statistics
      geometries->center += v0 + v1 + v2;
      total_upper_bound = glm::max(total_upper_bound, v0);
      total_upper_bound = glm::max(total_upper_bound, v1);
      total_upper_bound = glm::max(total_upper_bound, v2);
      total_lower_bound = glm::min(total_lower_bound, v0);
      total_lower_bound = glm::min(total_lower_bound, v1);
      total_lower_bound = glm::min(total_lower_bound, v2);

      // Update
      index_offset += fv;
    }

    // Loop again to normalize normals
    if (computing_normals) {
      index_offset = 0;
      for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
        int fv = shapes[s].mesh.num_face_vertices[f];

        tinyobj::index_t& idx0 = shapes[s].mesh.indices[index_offset + 0];
        tinyobj::index_t& idx1 = shapes[s].mesh.indices[index_offset + 1];
        tinyobj::index_t& idx2 = shapes[s].mesh.indices[index_offset + 2];

        glm::vec3 n0 = glm::normalize(glm::vec3(attrib.normals[3 * idx0.normal_index + 0],
                                                attrib.normals[3 * idx0.normal_index + 1],
                                                attrib.normals[3 * idx0.normal_index + 2]));
        glm::vec3 n1 = glm::normalize(glm::vec3(attrib.normals[3 * idx1.normal_index + 0],
                                                attrib.normals[3 * idx1.normal_index + 1],
                                                attrib.normals[3 * idx1.normal_index + 2]));
        glm::vec3 n2 = glm::normalize(glm::vec3(attrib.normals[3 * idx2.normal_index + 0],
                                                attrib.normals[3 * idx2.normal_index + 1],
                                                attrib.normals[3 * idx2.normal_index + 2]));

        assert(idx0.normal_index != -1);
        data_n[3 * (index_offset + 0) + 0] = n0.x;
        data_n[3 * (index_offset + 0) + 1] = n0.y;
        data_n[3 * (index_offset + 0) + 2] = n0.z;
        assert(idx1.normal_index != -1);
        data_n[3 * (index_offset + 1) + 0] = n1.x;
        data_n[3 * (index_offset + 1) + 1] = n1.y;
        data_n[3 * (index_offset + 1) + 2] = n1.z;
        assert(idx2.normal_index != -1);
        data_n[3 * (index_offset + 2) + 0] = n2.x;
        data_n[3 * (index_offset + 2) + 1] = n2.y;
        data_n[3 * (index_offset + 2) + 2] = n2.z;

        index_offset += fv;
      }
    }
  }

  // Information
  geometries->name = filename;
  geometries->center /= total_num_of_vertices; // Center of mass
  geometries->lower = total_lower_bound;
  geometries->upper = total_upper_bound;
  glm::vec3 s = total_upper_bound - total_lower_bound;
  geometries->scale = 4.f / glm::max(s.x, glm::max(s.y, s.z));

  return geometries;
}

template<>
TriangleArrayObjects*
ReadAsArrayObjects<false>(const std::string& inputfile)
{
  auto char_id = inputfile.find_last_of('/');
  auto directory = inputfile.substr(0, char_id);
  auto filename = inputfile.substr(char_id + 1, inputfile.size() - char_id - 1);

  auto* geometries = new TriangleArrayObjects();

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn;
  std::string err;

  bool ret = tinyobj::LoadObj(
    &attrib, &shapes, &materials, &warn, &err, inputfile.c_str(), directory.c_str(), true /* force triangulate */);

  if (!warn.empty()) {
    std::cout << warn << std::endl;
  }

  if (!err.empty()) {
    std::cerr << err << std::endl;
  }

  if (!ret) {
    throw std::runtime_error("failed to read file " + inputfile);
  }

  size_t total_num_of_vertices = 0;
  glm::vec3 total_lower_bound(+std::numeric_limits<float>::max());
  glm::vec3 total_upper_bound(-std::numeric_limits<float>::max());

  bool computing_normals = false;
  if (attrib.normals.empty()) {
    computing_normals = true;
    // compute per vertex normal
    attrib.normals = std::vector<tinyobj::real_t>(attrib.vertices.size(), 0.f);
  }

  // Loop over shapes
  for (size_t s = 0; s < shapes.size(); s++) {

    // Compute size
    const size_t num_of_vertices = std::accumulate(shapes[s].mesh.num_face_vertices.begin(),
                                                   shapes[s].mesh.num_face_vertices.end(),
                                                   size_t(0),
                                                   [](size_t init, uint8_t value) { return init + value; });
    total_num_of_vertices += num_of_vertices;

    // Create a geometry
    geometries->meshes.emplace_back();
    auto& g = geometries->meshes.back();
    g.size_triangles = num_of_vertices / 3;
    g.vertices.reset(new float[g.size_triangles * 9]);
    g.normals.reset(new float[g.size_triangles * 9]() /* zero initialize */);
    auto* data_v = g.vertices.get();
    auto* data_n = g.normals.get();

    // Loop over faces (polygon)
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {

      // Verify
      int fv = shapes[s].mesh.num_face_vertices[f];
      if (fv != 3)
        throw std::runtime_error("non-triangle faces are not supported");

      // Access to vertex
      tinyobj::index_t& idx0 = shapes[s].mesh.indices[index_offset + 0];
      tinyobj::index_t& idx1 = shapes[s].mesh.indices[index_offset + 1];
      tinyobj::index_t& idx2 = shapes[s].mesh.indices[index_offset + 2];
      glm::vec3 v0 = glm::vec3(attrib.vertices[3 * idx0.vertex_index + 0],
                               attrib.vertices[3 * idx0.vertex_index + 1],
                               attrib.vertices[3 * idx0.vertex_index + 2]);
      glm::vec3 v1 = glm::vec3(attrib.vertices[3 * idx1.vertex_index + 0],
                               attrib.vertices[3 * idx1.vertex_index + 1],
                               attrib.vertices[3 * idx1.vertex_index + 2]);
      glm::vec3 v2 = glm::vec3(attrib.vertices[3 * idx2.vertex_index + 0],
                               attrib.vertices[3 * idx2.vertex_index + 1],
                               attrib.vertices[3 * idx2.vertex_index + 2]);

      // Fill the vertex buffer
      data_v[3 * (index_offset + 0) + 0] = v0.x;
      data_v[3 * (index_offset + 0) + 1] = v0.y;
      data_v[3 * (index_offset + 0) + 2] = v0.z;
      data_v[3 * (index_offset + 1) + 0] = v1.x;
      data_v[3 * (index_offset + 1) + 1] = v1.y;
      data_v[3 * (index_offset + 1) + 2] = v1.z;
      data_v[3 * (index_offset + 2) + 0] = v2.x;
      data_v[3 * (index_offset + 2) + 1] = v2.y;
      data_v[3 * (index_offset + 2) + 2] = v2.z;

      // Fill the normal buffer
      if (!computing_normals) {
        assert(idx0.normal_index != -1);
        data_n[3 * (index_offset + 0) + 0] = attrib.normals[3 * idx0.normal_index + 0];
        data_n[3 * (index_offset + 0) + 1] = attrib.normals[3 * idx0.normal_index + 1];
        data_n[3 * (index_offset + 0) + 2] = attrib.normals[3 * idx0.normal_index + 2];
        assert(idx1.normal_index != -1);
        data_n[3 * (index_offset + 1) + 0] = attrib.normals[3 * idx1.normal_index + 0];
        data_n[3 * (index_offset + 1) + 1] = attrib.normals[3 * idx1.normal_index + 1];
        data_n[3 * (index_offset + 1) + 2] = attrib.normals[3 * idx1.normal_index + 2];
        assert(idx2.normal_index != -1);
        data_n[3 * (index_offset + 2) + 0] = attrib.normals[3 * idx2.normal_index + 0];
        data_n[3 * (index_offset + 2) + 1] = attrib.normals[3 * idx2.normal_index + 1];
        data_n[3 * (index_offset + 2) + 2] = attrib.normals[3 * idx2.normal_index + 2];
      }
      else {
        // Compute Normal (flat shading)
        glm::vec3 e10 = v1 - v0;
        glm::vec3 e20 = v2 - v0;
        glm::vec3 N = glm::cross(e10, e20);
        data_n[3 * (index_offset + 0) + 0] = N.x;
        data_n[3 * (index_offset + 0) + 1] = N.y;
        data_n[3 * (index_offset + 0) + 2] = N.z;
        data_n[3 * (index_offset + 1) + 0] = N.x;
        data_n[3 * (index_offset + 1) + 1] = N.y;
        data_n[3 * (index_offset + 1) + 2] = N.z;
        data_n[3 * (index_offset + 2) + 0] = N.x;
        data_n[3 * (index_offset + 2) + 1] = N.y;
        data_n[3 * (index_offset + 2) + 2] = N.z;
      }

      // Statistics
      geometries->center += v0 + v1 + v2;
      total_upper_bound = glm::max(total_upper_bound, v0);
      total_upper_bound = glm::max(total_upper_bound, v1);
      total_upper_bound = glm::max(total_upper_bound, v2);
      total_lower_bound = glm::min(total_lower_bound, v0);
      total_lower_bound = glm::min(total_lower_bound, v1);
      total_lower_bound = glm::min(total_lower_bound, v2);

      // Update
      index_offset += fv;
    }
  }

  // Information
  geometries->name = filename;
  geometries->center /= total_num_of_vertices; // Center of mass
  geometries->lower = total_lower_bound;
  geometries->upper = total_upper_bound;
  glm::vec3 s = total_upper_bound - total_lower_bound;
  geometries->scale = 4.f / glm::max(s.x, glm::max(s.y, s.z));

  return geometries;
}

void
TriangleIndexObjects::Clear()
{
  glDeleteBuffers(1, &vbo_vertex);
  glDeleteBuffers(1, &vbo_normal);
  for (auto& m : meshes) {
    glDeleteBuffers(1, &m.vbo_index);
  }
}

void
TriangleArrayObjects::Clear()
{
  for (auto& m : meshes) {
    glDeleteBuffers(1, &m.vbo_vertex);
    glDeleteBuffers(1, &m.vbo_normal);
  }
}

void
TriangleIndexObjects::Create()
{
  glGenBuffers(1, &vbo_vertex);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size_of_buffer, vertices.get(), GL_STATIC_DRAW);

  glGenBuffers(1, &vbo_normal);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size_of_buffer, normals.get(), GL_STATIC_DRAW);

  // Generate a buffer for the indices as well
  for (auto& m : meshes) {
    glGenBuffers(1, &m.vbo_index);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.vbo_index);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.size_indices * sizeof(unsigned int), m.indices.get(), GL_STATIC_DRAW);
  }
}

void
TriangleArrayObjects::Create()
{
  // Generate a buffer for the indices as well
  for (auto& m : meshes) {
    glGenBuffers(1, &m.vbo_vertex);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo_vertex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m.size_triangles * 9, m.vertices.get(), GL_STATIC_DRAW);
    glGenBuffers(1, &m.vbo_normal);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m.size_triangles * 9, m.normals.get(), GL_STATIC_DRAW);
  }
}

void
TriangleIndexObjects::Render()
{
  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex);
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
  glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
  glVertexAttribPointer(1, // attribute. No particular reason for 1, but must
                           // match the layout in the shader.
                        3, // size
                        GL_FLOAT, // type
                        GL_FALSE, // normalized?
                        0, // stride
                        (void*)0 // array buffer offset
  );

  for (auto& m : meshes) {
    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.vbo_index);

    // Draw the triangles !
    glDrawElements(GL_TRIANGLES, // mode
                   m.size_indices, // count
                   GL_UNSIGNED_INT, // type
                   (void*)0 // element array buffer offset
    );
  }

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}

void
TriangleArrayObjects::Render()
{
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  for (auto& m : meshes) {
    // 1rst attribute buffer : vertices
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo_vertex);
    glVertexAttribPointer(0, // attribute. No particular reason for 0, but must
                             // match the layout in the shader.
                          3, // size
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0, // stride
                          (void*)0 // array buffer offset
    );

    // 2nd attribute buffer : colors
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo_normal);
    glVertexAttribPointer(1, // attribute. No particular reason for 1, but must
                             // match the layout in the shader.
                          3, // size
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0, // stride
                          (void*)0 // array buffer offset
    );

    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, m.size_triangles * 3);
  }

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}
