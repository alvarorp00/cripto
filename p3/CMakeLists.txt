# Global

cmake_minimum_required(VERSION 3.10)

# External

include(ExternalProject)

# project name

project(p2 C)
set(CMAKE_C_COMPILER "cc")

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

## Libsodium

ExternalProject_add(
  LIBSODIUM
  PREFIX ${EXTERNAL_INSTALL_LOCATION}/libsodium
  URL https://download.libsodium.org/libsodium/releases/libsodium-1.0.17-stable.tar.gz
  CONFIGURE_COMMAND sh -c "CC=gcc ../LIBSODIUM/configure"
  BUILD_COMMAND make -j4 && make check
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
)

ExternalProject_Get_property(LIBSODIUM BINARY_DIR)
set(LIBLIBSODIUM ${BINARY_DIR}/src/libsodium/.libs/libsodium.a)

ExternalProject_Get_property( LIBSODIUM SOURCE_DIR )
include_directories( ${SOURCE_DIR} )

## gmp

ExternalProject_add(
  GMP
  PREFIX ${EXTERNAL_INSTALL_LOCATION}/gmp
  URL https://gmplib.org/download/gmp/gmp-6.2.1.tar.lz
  CONFIGURE_COMMAND sh -c "CC=gcc ../GMP/configure --prefix=$(pwd)"
  BUILD_COMMAND make -j4
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
)

ExternalProject_Get_property(GMP BINARY_DIR)
set(LIBGMP ${BINARY_DIR}/.libs/libgmp.a)

include_directories(${BINARY_DIR})

# subdirectories

add_subdirectory(rsa ${EXECUTABLE_OUTPUT_PATH})
