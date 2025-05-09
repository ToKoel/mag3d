cmake_minimum_required(VERSION 3.16)
project(magneticVis)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# ---- SDL2 ----
FetchContent_Declare(
  SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-2.30.2
)
FetchContent_MakeAvailable(SDL2)

# ---- ImGui ----
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG master
)
FetchContent_MakeAvailable(imgui)

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
)


target_sources(${PROJECT_NAME} PRIVATE
    src/gui_handler.cpp
${imgui_SOURCE_DIR}/imgui.cpp
${imgui_SOURCE_DIR}/imgui_draw.cpp
${imgui_SOURCE_DIR}/imgui_tables.cpp
${imgui_SOURCE_DIR}/imgui_widgets.cpp
${imgui_SOURCE_DIR}/imgui_demo.cpp
${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

if(APPLE)
  find_library(COCOA_LIBRARY Cocoa REQUIRED)
  find_library(OpenGL_LIBRARY OpenGL REQUIRED)
  target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 ${OpenGL_LIBRARY} ${COCOA_LIBRARY})
endif()

