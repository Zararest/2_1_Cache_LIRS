cmake_minimum_required(VERSION 3.0.0)
project(Cache_ARC VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(Cache_ARC main.cpp cache.cpp cache.inl)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
