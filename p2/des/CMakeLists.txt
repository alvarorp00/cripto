# des config

# dependencies

set(DES_INC code/inc)
set(DES_SRC code/src)

set(LIBS "-lm -lcrypto")
set(GMPLIB "-lgmp")
# set(LIBLIBSODIUM "-lsodium")

file(GLOB sources ${DES_SRC}/*.c)

include_directories(${DES_INC})

# link libraries

add_compile_options(${LIBS} ${GMPLIB})

# executable

add_executable(des
  code/des_main.c
  ${sources}
)

add_executable(tdea
  code/main_3DES.c
  ${sources}
)

target_link_libraries(des ${LIBLIBSODIUM})
target_link_libraries(tdea ${LIBLIBSODIUM})