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
  OpenCV
  glfw3
  OpenGL
)

set(macos_deps)
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  minipkg_import(ApplicationServices)
  list(APPEND macos_deps minipkg::ApplicationServices)

  minipkg_import(Cocoa)
  list(APPEND macos_deps minipkg::Cocoa)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()

#--- Portable File Dialogs
set(portable_file_dialogs_src_dir "/Users/zz/work/github/portable-file-dialogs")
add_subdirectory(${portable_file_dialogs_src_dir} ${CMAKE_BINARY_DIR}/portable_file_dialogs)

target_link_libraries(cap PRIVATE
  minipkg::opencv_imgcodecs
  minipkg::opencv_imgproc
  minipkg::opencv_core
  portable_file_dialogs
  ${macos_deps}
  minipkg::glfw3
  minipkg::OpenGL
)

if(MSVC)
  get_filename_component(OpenCV_BIN_DIR "${OpenCV_DIR}/../bin" ABSOLUTE)  
  set_target_properties(
    cap PROPERTIES
    VS_DEBUGGER_ENVIRONMENT "PATH=${OpenCV_BIN_DIR};%PATH%"
  )
endif()