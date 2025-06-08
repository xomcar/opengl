#include "shaders.h"
#include <stdio.h>
#include <stdlib.h>

ShaderLoadResult ShaderLoadFromDisk(const char *vertex_path, const char *fragment_path, GLuint *shader_output) {
  // Read vertex shader file
  FILE *file = fopen(vertex_path, "rb");
  if (!file)
    return FAILED_VERTEX_NOT_FOUND;
  fseek(file, 0, SEEK_END);
  long vertex_length = ftell(file);
  rewind(file);
  char *vertex_source = malloc(vertex_length + 1);
  if (!vertex_source) {
    fclose(file);
    return FAILED_VERTEX_NOT_FOUND;
  }
  fread(vertex_source, 1, vertex_length, file);
  vertex_source[vertex_length] = '\0';
  fclose(file);

  // Read fragment shader file
  file = fopen(fragment_path, "rb");
  if (!file) {
    free(vertex_source);
    return FAILED_FRAGMENT_NOT_FOUND;
  }
  fseek(file, 0, SEEK_END);
  long fragment_length = ftell(file);
  rewind(file);
  char *fragment_source = malloc(fragment_length + 1);
  if (!fragment_source) {
    free(vertex_source);
    fclose(file);
    return FAILED_FRAGMENT_NOT_FOUND;
  }
  fread(fragment_source, 1, fragment_length, file);
  fragment_source[fragment_length] = '\0';
  fclose(file);

  // Compile vertex shader
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  *shader_output = vertex_shader;
  glShaderSource(vertex_shader, 1, (const char **)&vertex_source, NULL);
  glCompileShader(vertex_shader);

  GLint success = 0;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glDeleteShader(vertex_shader);
    free(vertex_source);
    free(fragment_source);
    return FAILED_COMPILE_VERTEX;
  }

  // Compile fragment shader
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  *shader_output = fragment_shader;
  glShaderSource(fragment_shader, 1, (const char **)&fragment_source, NULL);
  glCompileShader(fragment_shader);

  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    free(vertex_source);
    free(fragment_source);
    return FAILED_COMPILE_FRAGMENT;
  }

  free(vertex_source);
  free(fragment_source);

  // Link shaders into a program
  GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteProgram(program);
    return FAILED_LINKAGE;
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  *shader_output = program;
  return SUCCESS;
}

void PrintShaderCompilationError(GLuint shader_handle) {
  GLint length = 0;
  glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &length);
  if (length > 1) {
    char *log = (char *)malloc(length);
    if (log) {
      glGetShaderInfoLog(shader_handle, length, NULL, log);
      free(log);
    }
  }
}

void PrintShaderLinkageError(GLuint program_shader_handle) {
  GLint length = 0;
  glGetProgramiv(program_shader_handle, GL_INFO_LOG_LENGTH, &length);
  if (length > 1) {
    char *log = (char *)malloc(length);
    if (log) {
      glGetProgramInfoLog(program_shader_handle, length, NULL, log);
      free(log);
    }
  }
}
