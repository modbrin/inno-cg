cmake_minimum_required(VERSION 3.8)
project(inno-cg)

# config
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "/Ot")

# libs are header-only
include_directories(lib/stb)
include_directories(lib/linalg)
include_directories(lib/catch2/single_include/catch2)
include_directories(src)

# custom function for building apps and tests
function(ADD_EXEC_TESTS)
    cmake_parse_arguments(PARSED "" "NAME" "DEPS" ${ARGN})

    # check if values are provided
    if(NOT PARSED_NAME)
        message(FATAL_ERROR "Target name is missing.")
    endif(NOT PARSED_NAME)

    if(NOT PARSED_DEPS)
        message(FATAL_ERROR "At least one translation unit is required.")
    endif(NOT PARSED_DEPS)

    # add main logic unit as library using key translation units for compilation
    add_library(${PARSED_NAME}_lib STATIC ${PARSED_DEPS})
    
    # add launchable executable
    add_executable(${PARSED_NAME} src/${PARSED_NAME}_main.cpp)
    target_link_libraries(${PARSED_NAME} ${PARSED_NAME}_lib)

    # add tests
    add_executable(${PARSED_NAME}_tests tests/${PARSED_NAME}_tests.cpp)
    target_link_libraries(${PARSED_NAME}_tests ${PARSED_NAME}_lib)
endfunction(ADD_EXEC_TESTS)

# 1) BLACK IMAGE --------------------------------------------------
ADD_EXEC_TESTS(NAME black_image DEPS src/black_image.cpp)

# 2) COLOR SPACE --------------------------------------------------
ADD_EXEC_TESTS(NAME color_space DEPS src/black_image.cpp
                                     src/color_space.cpp)

# 3) DRAW LINE  ---------------------------------------------------
ADD_EXEC_TESTS(NAME draw_line DEPS src/black_image.cpp
                                   src/color_space.cpp
                                   src/draw_line.cpp)

# 4) READ OBJ  ----------------------------------------------------
ADD_EXEC_TESTS(NAME read_obj DEPS src/black_image.cpp
                                  src/color_space.cpp
                                  src/draw_line.cpp
                                  src/read_obj.cpp)

# 5) PROJECTION  --------------------------------------------------
ADD_EXEC_TESTS(NAME projection DEPS src/black_image.cpp
                                    src/color_space.cpp
                                    src/draw_line.cpp
                                    src/read_obj.cpp
                                    src/projection.cpp)

# 6) TRIANGLE RASTERIZATION  --------------------------------------
ADD_EXEC_TESTS(NAME triangle_rasterization DEPS src/black_image.cpp
                                    src/color_space.cpp
                                    src/draw_line.cpp
                                    src/read_obj.cpp
                                    src/projection.cpp
                                    src/triangle_rasterization.cpp)

# 7) Z BUFFER CULLING  --------------------------------------------
ADD_EXEC_TESTS(NAME z_buffer_culling DEPS src/black_image.cpp
                                    src/color_space.cpp
                                    src/draw_line.cpp
                                    src/read_obj.cpp
                                    src/projection.cpp
                                    src/triangle_rasterization.cpp
                                    src/z_buffer_culling.cpp)

# 8) LIGHTING  ----------------------------------------------------
ADD_EXEC_TESTS(NAME lighting DEPS src/black_image.cpp
                                    src/color_space.cpp
                                    src/draw_line.cpp
                                    src/read_obj.cpp
                                    src/projection.cpp
                                    src/triangle_rasterization.cpp
                                    src/z_buffer_culling.cpp
                                    src/lighting.cpp)