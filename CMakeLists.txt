cmake_minimum_required(VERSION 3.5)
project(cap)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(cap
  main.cpp
  transparent_window.h
  transparent_window.mm
)

find_library(ApplicationServices_LIBRARY ApplicationServices REQUIRED)
find_library(ImageIO_LIBRARY ImageIO REQUIRED)
find_library(COCOA_LIBRARY Cocoa REQUIRED)
target_link_libraries(cap PRIVATE
  ${ApplicationServices_LIBRARY}
  ${ImageIO_LIBRARY}
  ${COCOA_LIBRARY}
)