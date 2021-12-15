# rsa config

# dependencies

set(RSA_INC code/inc)
set(RSA_SRC code/src)
set(RSA_MAIN code/mains)

set(LIBS "-lm -lcrypto -lgmp")
set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/gcc")

file(GLOB sources ${RSA_SRC}/*.c)
include_directories(${RSA_INC})

# link libraries

add_compile_options(${LIBS})

# executable

add_executable(potenciacion
  ${RSA_MAIN}/power_main.c
  ${sources}
)

add_executable(primo
  ${RSA_MAIN}/prime_main.c
  ${sources}
)

target_link_libraries(potenciacion ${LIBLIBSODIUM})
target_link_libraries(potenciacion ${LIBGMP})

target_link_libraries(primo ${LIBLIBSODIUM})
target_link_libraries(primo ${LIBGMP})
target_link_libraries(primo m)