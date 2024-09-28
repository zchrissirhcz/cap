cmake_minimum_required(VERSION 3.5)
project(cap)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(cap
  main.cpp
  cap.h
  $<$<CXX_COMPILER_ID:MSVC>:cap_windows.cpp>
  $<$<CXX_COMPILER_ID:AppleClang>:cap_macos.cpp>
  imgproc.hpp
  imgproc.cpp
  clipboard.hpp
  $<$<CXX_COMPILER_ID:MSVC>:clipboard_windows.cpp>
  $<$<CXX_COMPILER_ID:AppleClang>:clipboard_macos.mm>
  filedialog.hpp
  filedialog.cpp
)

include(minipkg.cmake)

minipkg_import(
  OpenCV@native
  glfw3@native
  OpenGL@native
)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  minipkg_import(
    ApplicationServices@native
    Cocoa@native
  )
  set(macos_deps native::ApplicationServices native::Cocoa)
  add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()

minipkg_import(portable-file-dialogs@minipkg)

target_link_libraries(cap PRIVATE
  native::opencv_imgcodecs
  native::opencv_imgproc
  native::opencv_core
  minipkg::portable-file-dialogs
  ${macos_deps}
  native::glfw3
  native::OpenGL
)

if(MSVC)
  get_filename_component(OpenCV_BIN_DIR "${OpenCV_DIR}/../bin" ABSOLUTE)  
  set_target_properties(
    cap PROPERTIES
    VS_DEBUGGER_ENVIRONMENT "PATH=${OpenCV_BIN_DIR};%PATH%"
  )
endif()