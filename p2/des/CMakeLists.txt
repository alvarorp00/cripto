# des config

# dependencies

set(DES_INC code/inc)
set(DES_SRC code/src)
set(DES_TABLE tables)

set(LIBS "-lm -lcrypto")
set(GMPLIB "-lgmp")



file(GLOB sources ${DES_SRC}/*.c ${DES_TABLE}/*.c)

include_directories(${DES_INC})

# link libraries

# target_link_libraries( ${LIBS} ${GMPLIB})

add_compile_options(${LIBS} ${GMPLIB})

# executable

add_executable(des
  code/des_main.c

  ${sources}
)