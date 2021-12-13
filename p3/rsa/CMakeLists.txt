# rsa config

# dependencies

set(RSA_INC code/inc)
set(RSA_SRC code/src)
set(RSA_MAIN code/mains)

set(LIBS "-lm -lcrypto -lgmp")

file(GLOB sources ${RSA_SRC}/*.c)
include_directories(${RSA_INC})

# link libraries

add_compile_options(${LIBS} ${GMPLIB})

# executable

add_executable(potenciacion
  ${RSA_MAIN}/power_main.c
  ${sources}
)

target_link_libraries(potenciacion ${LIBLIBSODIUM})
target_link_libraries(potenciacion ${LIBGMP})