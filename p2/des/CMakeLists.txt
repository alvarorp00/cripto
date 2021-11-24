# des config

# dependencies

set(DES_INC code/inc)
set(DES_SRC code/src)
set(DES_MAIN code/mains)

set(LIBS "-lm -lcrypto")
set(GMPLIB "-lgmp")
# set(LIBLIBSODIUM "-lsodium")

file(GLOB sources ${DES_SRC}/*.c)
include_directories(${DES_INC})

# link libraries

add_compile_options(${LIBS} ${GMPLIB})

# executable

add_executable(desCFB
  ${DES_MAIN}/des_main.c
  ${sources}
)

add_executable(TDEA-CFB
  ${DES_MAIN}/tdea_main.c
  ${sources}
)

add_executable(linearitySB
  ${DES_MAIN}/linearity_main.c
  ${sources}
)

target_link_libraries(desCFB ${LIBLIBSODIUM})
target_link_libraries(TDEA-CFB ${LIBLIBSODIUM})
target_link_libraries(linearitySB ${LIBLIBSODIUM})