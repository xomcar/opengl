#include "image.h"
#include "shaders.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

typedef enum Color { STILL, GRADIENT } ShaderColor;
typedef enum Shape { TRI, RECT } Shape;

static enum Color color = STILL;
static enum Shape shape = RECT;
static GLenum mode = GL_FILL;
static float texture_mix = 0.2;

void frameBufferSizeCallback(GLFWwindow *window, int width, int height) { glViewport(0, 0, width, height); }

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  static bool wKeyWasPressed = false;
  if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) && !wKeyWasPressed) {
    mode = mode == GL_LINE ? GL_FILL : GL_LINE;
  }

  static bool cKeyWasPressed = false;
  if ((glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) && !cKeyWasPressed) {
    color = (color == STILL) ? GRADIENT : STILL;
  }

  static bool sKeyWasPressed = false;
  if ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) && !sKeyWasPressed) {
    shape = (shape == TRI) ? RECT : TRI;
  }

  static bool upKeyWasPressed = false;
  if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) && !upKeyWasPressed) {
    texture_mix += 0.1f;
    if (texture_mix > 1.0f) {
      texture_mix = 1.0f;
    }
  }

  static bool downKeyWasPressed = false;
  if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && !downKeyWasPressed) {
    texture_mix -= 0.1f;
    if (texture_mix < 0.0f) {
      texture_mix = 0.0f;
    }
  }

  wKeyWasPressed = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) ? true : false;
  cKeyWasPressed = (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) ? true : false;
  sKeyWasPressed = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) ? true : false;
  upKeyWasPressed = (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) ? true : false;
  downKeyWasPressed = (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) ? true : false;
}

int main() {
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    printf("Failed to create GLFW window\n");
    return -1;
  }
  glfwMakeContextCurrent(window);
  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

  float rectangle_vp[] = {
      // positions          // colors           // texture coords
      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
  };

  GLfloat triangle_vp[] = {
      // positions         // colors
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
      0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // top
  };

  GLuint indices[] = {0, 1, 3, 1, 2, 3};

  GLuint fShader;
  ShaderLoadResult res;
  res = ShaderLoadFromDisk("shaders/fixed.vertex.glsl", "shaders/fixed.fragment.glsl", &fShader);
  assert(res == SUCCESS && "Failed to compile fixed shader");
  GLuint tShader;
  res = ShaderLoadFromDisk("shaders/texture.vertex.glsl", "shaders/texture.fragment.glsl", &tShader);
  assert(res == SUCCESS && "Failed to compile texture shader");

  GLint tex0Location = glGetUniformLocation(tShader, "texture0");
  GLint tex1Location = glGetUniformLocation(tShader, "texture1");
  GLint mixAmountLocation = glGetUniformLocation(tShader, "mixAmount");

  GLuint VBO_rect;
  GLuint VBO_tria;
  GLuint EBO_rect;
  GLuint VAO_rect;
  GLuint VAO_tri;
  glGenBuffers(1, &VBO_rect);
  glGenBuffers(1, &VBO_tria);
  glGenBuffers(1, &EBO_rect);
  glGenVertexArrays(1, &VAO_rect);
  glGenVertexArrays(1, &VAO_tri);

  // Configure rectangle VAO
  glBindVertexArray(VAO_rect); // vertex array must be bound before buffers! attributes are linked to a buffer
  glBindBuffer(GL_ARRAY_BUFFER, VBO_rect);
  glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vp), rectangle_vp, GL_STATIC_DRAW);

  // An EBO stores indices of what vertices (contained in a VBO) to draw (indexed drawing)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_rect);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
  glEnableVertexAttribArray(0); // select defined vertex array to parse the VBO buffer

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // Load textures
  int width, height, nrChannels;

  GLuint texture0;
  glGenTextures(1, &texture0);
  glBindTexture(GL_TEXTURE_2D, texture0);

  // stbi_set_flip_vertically_on_load(true);
  unsigned char *wood = stbi_load("data/container.jpg", &width, &height, &nrChannels, 0);
  assert(wood != NULL && "Failed to load wood texture");
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, wood);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(wood);

  GLuint texture1;
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);

  unsigned char *face = stbi_load("data/awesomeface.png", &width, &height, &nrChannels, 0);
  assert(face != NULL && "Failed to load face texture");

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, face);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(face);

  // Configure triangle VAO
  glBindVertexArray(VAO_tri);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_tria);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vp), triangle_vp, GL_STATIC_DRAW);

  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
  glEnableVertexAttribArray(0);

  // color
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0); // reset bound vao

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, mode);

    if (shape == TRI) {
      glUseProgram(fShader);
      glBindVertexArray(VAO_tri);
      glDrawArrays(GL_TRIANGLES, 0, 3);
    } else {
      glUniform1i(tex0Location, 0);
      glUniform1i(tex1Location, 1);
      glUniform1f(mixAmountLocation, texture_mix);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture0);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, texture1);
      glUseProgram(tShader);
      glBindVertexArray(VAO_rect);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    processInput(window);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}
