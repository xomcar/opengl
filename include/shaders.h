#ifndef SHADERS_H
#define SHADERS_H

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>

typedef enum ShaderLoadResult {
  SUCCESS,
  FAILED_VERTEX_NOT_FOUND,
  FAILED_FRAGMENT_NOT_FOUND,
  FAILED_COMPILE_VERTEX,
  FAILED_COMPILE_FRAGMENT,
  FAILED_LINKAGE
} ShaderLoadResult;

ShaderLoadResult ShaderLoadFromDisk(const char *vertex_path, const char *fragment_path, GLuint *shader_output_program);
void PrintShaderCompilationError(GLuint shader_handle);
void PrintShaderLinkageError(GLuint program_shader_handle);
#endif