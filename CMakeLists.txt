cmake_minimum_required(VERSION 2.8)
find_package(Boost 1.40 COMPONENTS program_options REQUIRED)

add_executable(Cache_LIRS src/main.cpp src/headers/cache.hpp)

add_executable(Cache_compare src/compare.cpp src/headers/cache.hpp src/headers/perfect_cache.hpp)

add_executable(Cache_test src/test.cpp src/headers/cache.hpp)

target_link_libraries(Cache_test Boost::program_options)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
