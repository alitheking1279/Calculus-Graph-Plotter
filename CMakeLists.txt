cmake_minimum_required(VERSION 3.10)
project(Calculus)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------
# Source Files
# -------------------
set(SOURCES
    main.cpp
    stb.cpp
    lib/glad/src/glad.c
    tinyexpr.c        # ✅ Add tinyexpr source here
)

# -------------------
# Include Directories
# -------------------
include_directories(
    ${CMAKE_SOURCE_DIR}/include                  # glm, stb
    ${CMAKE_SOURCE_DIR}/lib/glad/include
    ${CMAKE_SOURCE_DIR}/lib/glfw/include
    ${CMAKE_SOURCE_DIR}/lib/assimp/include       # Assimp headers
    ${CMAKE_SOURCE_DIR}                           # For tinyexpr.h
)

# -------------------
# Library Directories
# -------------------
link_directories(
    ${CMAKE_SOURCE_DIR}/lib/glfw/lib-vc2022
    ${CMAKE_SOURCE_DIR}/lib/assimp/lib
)

# -------------------
# Build Executable
# -------------------
add_executable(${PROJECT_NAME} ${SOURCES})

# -------------------
# Link Libraries
# -------------------
if (WIN32)
    target_link_libraries(${PROJECT_NAME}
        ${CMAKE_SOURCE_DIR}/lib/glfw/lib-vc2022/glfw3.lib
        ${CMAKE_SOURCE_DIR}/lib/assimp/lib/assimp.lib
        opengl32
    )
else()
    target_link_libraries(${PROJECT_NAME}
        glfw
        assimp
        GL
        dl
        pthread
    )
endif()
