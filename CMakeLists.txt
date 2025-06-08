cmake_minimum_required(VERSION 3.11)
project(GLFWExample C)

set(CMAKE_C_STANDARD 99)
set(OpenGL_GL_PREFERENCE GLVND) # use modern

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${GLFW_INCLUDE_DIRS})

file(GLOB SOURCES "src/*.c")

add_executable(main ${SOURCES})

target_link_libraries(main glfw OpenGL::GL)
