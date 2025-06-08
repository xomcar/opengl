#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>

typedef enum Color { ORANGE, PINK } ShaderColor;
typedef enum Shape { TRI, RECT } Shape;

static enum Color color = ORANGE;
static enum Shape shape = TRI;
static GLenum mode = GL_FILL;

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
    color = (color == ORANGE) ? PINK : ORANGE;
  }

  static bool sKeyWasPressed = false;
  if ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) && !sKeyWasPressed) {
    shape = (shape == TRI) ? RECT : TRI;
  }

  wKeyWasPressed = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) ? true : false;
  cKeyWasPressed = (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) ? true : false;
  sKeyWasPressed = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) ? true : false;
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

  GLfloat rectangle_vp[] = {
      0.5f,  0.5f,  0.0f, // top right
      0.5f,  -0.5f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, // bottom left
      -0.5f, 0.5f,  .0f   // top left
  };

  GLfloat triangle_vp[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

  GLuint indices[] = {0, 1, 3, 1, 2, 3};

  const char *vertexShaderSource = "#version 330 core\n"
                                   "layout (location = 0) in vec3 aPos;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   gl_Position = vec4(aPos, 1.0);\n"
                                   "}\0";

  const char *fragmentShaderSources[] = {"#version 330 core\n"
                                         "out vec4 FragColor;\n"
                                         "void main()\n"
                                         "{\n "
                                         "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                         "}\0",
                                         "#version 330 core\n"
                                         "out vec4 FragColor;\n"
                                         "void main()\n"
                                         "{\n "
                                         "    FragColor = vec4(1.0f, 0.5f, 0.7f, 1.0f);\n"
                                         "}\0"};

  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

  glCompileShader(vertexShader);

  unsigned int orangeFragShader;
  orangeFragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(orangeFragShader, 1, &fragmentShaderSources[0], NULL);

  unsigned int pinkFragShader;
  pinkFragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(pinkFragShader, 1, &fragmentShaderSources[1], NULL);

  glCompileShader(orangeFragShader);
  glCompileShader(pinkFragShader);

  unsigned int orangeShaderProgram;
  orangeShaderProgram = glCreateProgram();
  glAttachShader(orangeShaderProgram, vertexShader);
  glAttachShader(orangeShaderProgram, orangeFragShader);
  glLinkProgram(orangeShaderProgram);

  unsigned int pinkShaderProgram;
  pinkShaderProgram = glCreateProgram();
  glAttachShader(pinkShaderProgram, vertexShader);
  glAttachShader(pinkShaderProgram, pinkFragShader);
  glLinkProgram(pinkShaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(orangeFragShader);
  glDeleteShader(pinkFragShader);

  unsigned int VBO_rect;
  unsigned int VBO_tria;
  unsigned int EBO_rect;
  unsigned int VAO_rect;
  unsigned int VAO_tri;
  glGenBuffers(1, &VBO_rect);
  glGenBuffers(1, &VBO_tria);
  glGenBuffers(1, &EBO_rect);
  glGenVertexArrays(1, &VAO_rect);
  glGenVertexArrays(1, &VAO_tri);

  // Configure rectangle VAO
  glBindVertexArray(VAO_rect); // vertex array must be bound before buffers! attributes are linked to a buffer
  glBindBuffer(GL_ARRAY_BUFFER, VBO_rect);
  glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vp), rectangle_vp, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

  // An EBO stores indices of what vertices (contained in a VBO) to draw (indexed drawing)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_rect);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0); // select defined vertex array to parse the VBO buffer

  // Configure triangle VAO
  glBindVertexArray(VAO_tri);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_tria);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vp), triangle_vp, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

  glEnableVertexAttribArray(0); // select defined vertex array to parse the VBO buffer

  glBindVertexArray(0); // reset bound vao

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, mode);

    if (color == PINK) {
      glUseProgram(pinkShaderProgram);

    } else {
      glUseProgram(orangeShaderProgram);
    }

    if (shape == TRI) {
      glBindVertexArray(VAO_tri);
      glDrawArrays(GL_TRIANGLES, 0, 3);
    } else {
      glBindVertexArray(VAO_rect);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    processInput(window);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}
