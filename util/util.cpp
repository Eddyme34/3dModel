//===========================================================================//
//                                                                           //
// Copyright(c) 2018 Qi Wu (Wilson)                                          //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#include "util.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// trying this obj loader https://github.com/syoyo/tinyobjloader
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"

#include <fstream>
#include <string>

//---------------------------------------------------------------------------------------
// error check helper from EPFL ICG class
static inline const char*
ErrorString(GLenum error)
{
  const char* msg;
  switch (error) {
#define Case(Token) \
  case Token:       \
    msg = #Token;   \
    break;
    Case(GL_INVALID_ENUM);
    Case(GL_INVALID_VALUE);
    Case(GL_INVALID_OPERATION);
    Case(GL_INVALID_FRAMEBUFFER_OPERATION);
    Case(GL_NO_ERROR);
    Case(GL_OUT_OF_MEMORY);
#undef Case
  }
  return msg;
}

void
_glCheckError(const char* file, int line, const char* comment)
{
  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR) {
    fprintf(stderr, "ERROR: %s (file %s, line %i: %s).\n", comment, file, line, ErrorString(error));
  }
}

//---------------------------------------------------------------------------------------

void
SaveJPG(const std::string& fname, std::vector<uint8_t>& fb, int w, int h)
{
  const size_t nchannel = fb.size() / ((size_t)w * (size_t)h);
  if (nchannel == 3) {
    stbi_write_jpg(fname.c_str(), w, h, 3, fb.data(), 100);
  }
  else if (nchannel == 4) {
    const int& width = w;
    const int& height = h;
    uint8_t* pixels = new uint8_t[width * height * 3];
    int index = 0;
    for (int j = height - 1; j >= 0; --j) {
      for (int i = 0; i < width; ++i) {
        int ir = int(fb[4 * (i + j * width) + 0]);
        int ig = int(fb[4 * (i + j * width) + 1]);
        int ib = int(fb[4 * (i + j * width) + 2]);
        pixels[index++] = ir;
        pixels[index++] = ig;
        pixels[index++] = ib;
      }
    }
    stbi_write_jpg(fname.c_str(), width, height, 3, pixels, 100);
    delete[] pixels;
  }
  else {
    throw std::runtime_error("Unknown image type");
  }
}

GLuint
loadBMP_embeded(const unsigned char* content, size_t bytes)
{
  // Data read from the header of the BMP file
  unsigned char header[54];
  unsigned int dataPos;
  unsigned int imageSize;
  unsigned int width, height;

  // Read the header, i.e. the 54 first bytes

  // If less than 54 bytes are read, problem
  if (bytes < 54) {
    throw std::runtime_error("Not a correct BMP file 0\n");
  }
  std::copy(content, content + 54, header);

  // A BMP files always begins with "BM"
  if (header[0] != 'B' || header[1] != 'M') {
    throw std::runtime_error("Not a correct BMP file 1\n");
  }
  // Make sure this is a 24bpp file
  if (*(int*)&(header[0x1E]) != 0) {
    throw std::runtime_error("Not a correct BMP file 2\n");
  }
  if (*(int*)&(header[0x1C]) != 24) {
    throw std::runtime_error("Not a correct BMP file 3\n");
  }

  // Read the information about the image
  dataPos = *(int*)&(header[0x0A]);
  imageSize = *(int*)&(header[0x22]);
  width = *(int*)&(header[0x12]);
  height = *(int*)&(header[0x16]);

  // Actual RGB data
  const unsigned char* data = &content[54];

  // Create one OpenGL texture
  GLuint textureID;
  glGenTextures(1, &textureID);

  // "Bind" the newly created texture : all future texture functions will modify
  // this texture
  glBindTexture(GL_TEXTURE_2D, textureID);

  // Give the image to OpenGL
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

  // OpenGL has now copied the data.

  // Poor filtering, or ...
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  // ... nice trilinear filtering ...
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  // ... which requires mipmaps. Generate them automatically.
  glGenerateMipmap(GL_TEXTURE_2D);

  // Return the ID of the texture we just created
  return textureID;
}

GLuint
loadBMP_from_file(const char* imagepath)
{
  printf("Reading image %s\n", imagepath);

  // Data read from the header of the BMP file
  unsigned char header[54];
  unsigned int dataPos;
  unsigned int imageSize;
  unsigned int width, height;
  // Actual RGB data
  unsigned char* data;

  // Open the file
  FILE* file = fopen(imagepath, "rb");
  if (!file) {
    printf("%s could not be opened. Are you in the right directory ? Don't "
           "forget to read the FAQ !\n",
           imagepath);
    getchar();
    return 0;
  }

  // Read the header, i.e. the 54 first bytes

  // If less than 54 bytes are read, problem
  if (fread(header, 1, 54, file) != 54) {
    printf("Not a correct BMP file\n");
    fclose(file);
    return 0;
  }

  // A BMP files always begins with "BM"
  if (header[0] != 'B' || header[1] != 'M') {
    printf("Not a correct BMP file\n");
    fclose(file);
    return 0;
  }
  // Make sure this is a 24bpp file
  if (*(int*)&(header[0x1E]) != 0) {
    printf("Not a correct BMP file\n");
    fclose(file);
    return 0;
  }
  if (*(int*)&(header[0x1C]) != 24) {
    printf("Not a correct BMP file\n");
    fclose(file);
    return 0;
  }

  // Read the information about the image
  dataPos = *(int*)&(header[0x0A]);
  imageSize = *(int*)&(header[0x22]);
  width = *(int*)&(header[0x12]);
  height = *(int*)&(header[0x16]);

  // Some BMP files are misformatted, guess missing information
  if (imageSize == 0)
    imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
  if (dataPos == 0)
    dataPos = 54; // The BMP header is done that way

  // Create a buffer
  data = new unsigned char[imageSize];

  // Read the actual data from the file into the buffer
  fread(data, 1, imageSize, file);

  // Everything is in memory now, the file can be closed.
  fclose(file);

  // Create one OpenGL texture
  GLuint textureID;
  glGenTextures(1, &textureID);

  // "Bind" the newly created texture : all future texture functions will modify
  // this texture
  glBindTexture(GL_TEXTURE_2D, textureID);

  // Give the image to OpenGL
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

  // OpenGL has now copied the data. Free our own version
  delete[] data;

  // Poor filtering, or ...
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  // ... nice trilinear filtering ...
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  // ... which requires mipmaps. Generate them automatically.
  glGenerateMipmap(GL_TEXTURE_2D);

  // Return the ID of the texture we just created
  return textureID;
}

GLuint
loadTexture_from_file(const char* imagepath)
{
  printf("Reading image %s\n", imagepath);

  int width, height, channels;

  // Actual RGB data
  // stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(imagepath,
                                  &width,
                                  &height,
                                  &channels,
                                  STBI_rgb);

  // Create one OpenGL texture
  GLuint textureID;
  glGenTextures(1, &textureID);

  // "Bind" the newly created texture : all future texture functions will modify
  // this texture
  glBindTexture(GL_TEXTURE_2D, textureID);

  // Give the image to OpenGL
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

  // OpenGL has now copied the data. Free our own version
  delete[] data;

  // Poor filtering, or ...
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  // ... nice trilinear filtering ...
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  // ... which requires mipmaps. Generate them automatically.
  glGenerateMipmap(GL_TEXTURE_2D);

  // Return the ID of the texture we just created
  return textureID;
}

void
ReadFrame(GLFWwindow* window, std::vector<uint8_t>& buffer, int w, int h)
{
  const size_t nchannel = buffer.size() / ((size_t)w * (size_t)h);
  assert(nchannel == 3);

  // reading from the default framebuffer
  glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, &(buffer[0]));
  check_error_gl("Save a frame");
}

//---------------------------------------------------------------------------------------

void
CheckShaderCompilationLog(GLuint shader, const std::string& fname)
{
  GLint isCompiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE) {
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    // The maxLength includes the NULL character
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
    // Provide the infolog in whatever manor you deem best.
    // Exit with failure.
    glDeleteShader(shader); // Don't leak the shader.
    // show the message
    std::cerr << "compilation error for shader: " << fname << std::endl << errorLog.data() << std::endl;
  }
}

static const char*
ReadShaderFile(const char* fname)
{
  std::ifstream file(fname, std::ios::binary | std::ios::ate | std::ios::in);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  char* buffer = new char[size + 1];
  buffer[size] = '\0';
  if (!file.read(const_cast<char*>(buffer), size)) {
    fprintf(stderr, "Error: Cannot read file %s\n", fname);
    exit(-1);
  }
  return buffer;
}

GLuint
LoadProgram_FromFiles(const char* vshader_fname, const char* fshader_fname)
{
  fprintf(stdout, "[shader] reading vertex shader file %s\n", vshader_fname);
  fprintf(stdout, "[shader] reading fragment shader file %s\n", fshader_fname);
  GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
  {
    const char* vshader_text = ReadShaderFile(vshader_fname);
    glShaderSource(vshader, 1, &vshader_text, NULL);
    glCompileShader(vshader);
    CheckShaderCompilationLog(vshader, vshader_fname);
    check_error_gl("Compile Vertex Shaders");
  }
  GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
  {
    const char* fshader_text = ReadShaderFile(fshader_fname);
    glShaderSource(fshader, 1, &fshader_text, NULL);
    glCompileShader(fshader);
    CheckShaderCompilationLog(fshader, fshader_fname);
    check_error_gl("Compile Fragment Shaders");
  }
  GLuint program = glCreateProgram();
  if (glCreateProgram == 0)
    throw std::runtime_error("wrong program");
  glAttachShader(program, vshader);
  glAttachShader(program, fshader);
  check_error_gl("Compile Shaders: Attach");
  glLinkProgram(program);
  check_error_gl("Compile Shaders: Link");
  glUseProgram(program);
  check_error_gl("Compile Shaders: Final");
  return program;
}

GLuint
LoadProgram_FromEmbededTexts(const char* vshader_text,
                             long int vshader_size,
                             const char* fshader_text,
                             long int fshader_size)
{
  std::string vstr = "", fstr = "";
  for (int i = 0; i < vshader_size; ++i) {
    vstr += vshader_text[i];
  }
  for (int i = 0; i < fshader_size; ++i) {
    fstr += fshader_text[i];
  }
  const char* vptr = vstr.c_str();
  const char* fptr = fstr.c_str();
  GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
  {
    glShaderSource(vshader, 1, &vptr, NULL);
    glCompileShader(vshader);
    CheckShaderCompilationLog(vshader, "embeded vertex shader");
    check_error_gl("Compile Vertex Shaders");
  }
  GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
  {
    glShaderSource(fshader, 1, &fptr, NULL);
    glCompileShader(fshader);
    CheckShaderCompilationLog(fshader, "embeded fragment shader");
    check_error_gl("Compile Fragment Shaders");
  }
  GLuint program = glCreateProgram();
  if (glCreateProgram == 0)
    throw std::runtime_error("wrong program");
  glAttachShader(program, vshader);
  glAttachShader(program, fshader);
  check_error_gl("Compile Shaders: Attach");
  glLinkProgram(program);
  check_error_gl("Compile Shaders: Link");
  glUseProgram(program);
  check_error_gl("Compile Shaders: Final");
  return program;
}
