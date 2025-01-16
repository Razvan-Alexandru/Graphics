cmake_minimum_required(VERSION 3.11)

set(PROJECT_NAME project_name CACHE STRING "Project Name")
set(SOURCE_PATH /src/code CACHE PATH "Path to source files...")

project(${PROJECT_NAME})

find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Eigen3 REQUIRED)

set(LIBRARIES
    OpenGL::GL
    OpenGL::GLU
    GLEW
    glfw
    glm::glm
    Eigen3::Eigen
)

file(GLOB_RECURSE SRC LIST_DIRECTORIES false ${SOURCE_PATH}/*.cpp )

message(STATUS "LIST OF SOURCEFILES...")
string(REPLACE ";" "\n" SOURCE_LIST "${SRC}")
message(${SOURCE_LIST})

# Add _bin suffix to prevent name clash
add_executable("${PROJECT_NAME}_bin" ${SRC})

target_link_libraries("${PROJECT_NAME}_bin" ${LIBRARIES})

# Set output to use original input name
set_target_properties("${PROJECT_NAME}_bin"
    PROPERTIES
        OUTPUT_NAME "${PROJECT_NAME}"
        RUNTIME_OUTPUT_NAME "${PROJECT_NAME}"
)

