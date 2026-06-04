cmake_minimum_required(VERSION 3.16)
project(window_template)


set(SDLTTF_VENDORED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# This assumes the SDL source is available in vendored/SDL
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_net EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_shadercross-main EXCLUDE_FROM_ALL)

include_directories(SYSTEM include)

# Create your game executable target as usual
add_executable(window_template WIN32 src/client.c)
add_executable(server_template WIN32 src/server.c)
#add_executable(window_template WIN32 test/region_n_block.c)

# Link to the actual SDL3 library.
target_link_libraries(window_template PRIVATE SDL3::SDL3)
target_link_libraries(window_template PRIVATE SDL3_image::SDL3_image)
target_link_libraries(window_template PRIVATE SDL3_ttf::SDL3_ttf)
target_link_libraries(window_template PRIVATE SDL3_net::SDL3_net)
target_link_libraries(window_template PRIVATE SDL3_shadercross::SDL3_shadercross)

target_link_libraries(server_template PRIVATE SDL3::SDL3)
target_link_libraries(server_template PRIVATE SDL3_net::SDL3_net)
