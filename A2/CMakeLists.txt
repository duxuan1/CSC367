cmake_minimum_required(VERSION 3.10)

project(csc367a2)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory(googletest)

add_library(a2_options INTERFACE)
target_compile_features(a2_options INTERFACE cxx_std_11)

add_library(a2_filters
  filters.cpp
)

target_link_libraries(a2_filters
  PUBLIC ${CMAKE_THREAD_LIBS_INIT} a2_options
)

add_library(pgm
  pgm.cpp
)

target_link_libraries(a2_filters
  PUBLIC a2_options
)

add_executable(main
  main.cpp
  very_big_sample.cpp
  very_tall_sample.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/very_big_sample.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/very_tall_sample.cpp
)

target_link_libraries(main
  PUBLIC a2_filters pgm a2_options
)

target_include_directories(main
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(pgm_creator
  pgm_creator.cpp
)

target_link_libraries(pgm_creator
  PUBLIC pgm a2_options
)

add_executable(test_solution
  tests.cpp
)

target_link_libraries(test_solution
  PUBLIC pgm a2_filters gtest a2_options
)

add_custom_command(
  OUTPUT
    very_big_sample.cpp
    very_big_sample.h
    very_tall_sample.cpp
    very_tall_sample.h
  COMMAND $<TARGET_FILE:pgm_creator>
  DEPENDS pgm_creator
)

add_custom_target(gen_include_samples
  DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/very_big_sample.c
    ${CMAKE_CURRENT_BINARY_DIR}/very_big_sample.h
    ${CMAKE_CURRENT_BINARY_DIR}/very_tall_sample.c
    ${CMAKE_CURRENT_BINARY_DIR}/very_tall_sample.h
)
