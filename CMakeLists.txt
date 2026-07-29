cmake_minimum_required(VERSION 3.16)

project(CompGeom LANGUAGES CXX)

# =========================
# C++ standard
# =========================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile_commands.json (utile pour clang-tidy, IDE, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

# =========================
# Copy data folder
# =========================
file(COPY ${PROJECT_SOURCE_DIR}/data
     DESTINATION ${CMAKE_BINARY_DIR})

# =========================
# Sources
# =========================
file(GLOB_RECURSE SRC_FILES
     ${PROJECT_SOURCE_DIR}/src/*.cpp
     ${PROJECT_SOURCE_DIR}/src/*.h)

file(GLOB_RECURSE TEST_FILES
     ${PROJECT_SOURCE_DIR}/tests/*.cpp
     ${PROJECT_SOURCE_DIR}/tests/*.h)

file(GLOB PROJECT_FILES
     ${PROJECT_SOURCE_DIR}/projects/*.cpp)

# =========================
# Library
# =========================
add_library(geomLib ${SRC_FILES})

target_include_directories(geomLib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

target_compile_features(geomLib PUBLIC cxx_std_17)

# =========================
# Strict warnings (cross-platform)
# =========================
if (MSVC)
    target_compile_options(geomLib PUBLIC
        /W4            # high warning level
        /permissive-   # strict standard conformance
        /EHsc
        /Zc:__cplusplus
        /wd4996        # optional: silence deprecated warnings
    )
else()
    target_compile_options(geomLib PUBLIC
        -Wall
        -Wpedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Wmissing-include-dirs
    )
endif()

# =========================
# Sanitizers (Debug, GCC/Clang only)
# =========================
if (NOT MSVC)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(geomLib PUBLIC
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
        )
        target_link_options(geomLib PUBLIC
            -fsanitize=address,undefined
        )
    endif()
endif()

# =========================
# GoogleTest
# =========================
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# =========================
# Executables in projects/
# =========================
foreach(SRC ${PROJECT_FILES})
    get_filename_component(TARGET_NAME ${SRC} NAME_WE)

    add_executable(${TARGET_NAME} ${SRC})

    target_link_libraries(${TARGET_NAME}
        PRIVATE geomLib
    )
endforeach()

# =========================
# Tests executable
# =========================
add_executable(tests ${TEST_FILES})

target_link_libraries(tests
    PRIVATE
        geomLib
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)