# Global

cmake_minimum_required(VERSION 3.10)

# External

include(ExternalProject)

# project name

project(p2 C)
set(CMAKE_C_COMPILER "gcc")

add_definitions(-std=c11)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
add_compile_options(-g -Wall)

## common

set (EXTERNAL_INSTALL_LOCATION ${PROJECT_SOURCE_DIR}/external)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/executables)

set(COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/common)

## common

include_directories(${COMMON_DIR})

## alphabet

set(ALPHABET_DIR ${COMMON_DIR}/dictionary)
include_directories(${ALPHABET_DIR})

## nxjson

ExternalProject_add(
  NXJSON
  PREFIX ${EXTERNAL_INSTALL_LOCATION}/nxjson
  GIT_REPOSITORY https://github.com/alvarorp00/nxjson
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
)

ExternalProject_Get_property(NXJSON BINARY_DIR)
set (LIBJSON ${BINARY_DIR}/nxjson.a)

ExternalProject_Get_property(NXJSON SOURCE_DIR)
include_directories(${SOURCE_DIR})

## gmp

ExternalProject_add(
  GMP
  PREFIX ${EXTERNAL_INSTALL_LOCATION}/gmp
  URL https://gmplib.org/download/gmp/gmp-6.2.1.tar.lz
  CONFIGURE_COMMAND sh -c "CC=gcc ../GMP/configure --prefix=<INSTALL_DIR>"
  BUILD_COMMAND make -j4
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
)

ExternalProject_Get_property(GMP BINARY_DIR)
set(LIBGMP ${BINARY_DIR}/.lib/libgmp.a)

include_directories(${BINARY_DIR})

# subdirectories

add_subdirectory(des ${EXECUTABLE_OUTPUT_PATH})