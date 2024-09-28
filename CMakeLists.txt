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

set(deps)
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  find_library(ApplicationServices_LIBRARY ApplicationServices REQUIRED)
  find_library(COCOA_LIBRARY Cocoa REQUIRED)
  list(APPEND deps ${ApplicationServices_LIBRARY})
  list(APPEND deps ${COCOA_LIBRARY})
endif()

#--- OpenCV
if(MSVC)
  set(OpenCV_DIR "C:/pkgs/opencv/4.10.0/x64/vc16/lib")
endif()
find_package(OpenCV REQUIRED)

#--- GLFW
if(MSVC)
  set(glfw_src_dir "C:/work/glfw-3.4")
  option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
  option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
  option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
  option(GLFW_INSTALL "Generate installation target" OFF)
  option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
  add_subdirectory(${glfw_src_dir} ${CMAKE_BINARY_DIR}/glfw EXCLUDE_FROM_ALL)
else()
  find_package(glfw3 REQUIRED)
endif()

#--- OpenGL
find_package(OpenGL REQUIRED)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()

#--- Portable File Dialogs
set(portable_file_dialogs_src_dir "/Users/zz/work/github/portable-file-dialogs")
add_subdirectory(${portable_file_dialogs_src_dir} ${CMAKE_BINARY_DIR}/portable_file_dialogs)

target_link_libraries(cap PRIVATE
  ${OpenCV_LIBS}
  portable_file_dialogs
  ${deps}
  glfw
  ${OPENGL_LIBRARIES}
)

if(MSVC)
  get_filename_component(OpenCV_BIN_DIR "${OpenCV_DIR}/../bin" ABSOLUTE)  
  set_target_properties(
    cap PROPERTIES
    VS_DEBUGGER_ENVIRONMENT "PATH=${OpenCV_BIN_DIR};%PATH%"
  )
endif()