cmake_minimum_required(VERSION 2.8)
find_package(Boost 1.40 COMPONENTS program_options REQUIRED)

add_executable(Cache_LIRS main.cpp cache.inl perfect_cache.inl)

add_executable(Cache_test test.cpp cache.inl perfect_cache.inl)

target_link_libraries(Cache_test Boost::program_options)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
