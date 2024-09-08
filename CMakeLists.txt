cmake_minimum_required(VERSION 3.5)
project(cap)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(cap
  main.cpp
  cap.h
  cap.cpp
  imgproc.hpp
  imgproc.cpp
clipboard.hpp
  clipboard.mm
)

find_library(ApplicationServices_LIBRARY ApplicationServices REQUIRED)
find_library(COCOA_LIBRARY Cocoa REQUIRED)
find_package(OpenCV REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()

target_link_libraries(cap PRIVATE
  ${OpenCV_LIBS}
  ${ApplicationServices_LIBRARY}
  ${COCOA_LIBRARY}
  glfw
  OpenGL::GL
)
