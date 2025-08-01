cmake_minimum_required(VERSION 3.16)

project(yyz VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Test)
qt_standard_project_setup()

add_subdirectory(src/app)
add_subdirectory(src/page)
add_subdirectory(src/component)
add_subdirectory(src/common/utils)

# compile test example 开启测试支持
# option(BUILD_TEST "Build unit tests" ON)
# if (BUILD_TEST)

# endif()

enable_testing()
add_subdirectory(test/component)
