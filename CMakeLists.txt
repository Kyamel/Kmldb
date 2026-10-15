cmake_minimum_required(VERSION 3.10)
project(Kmldb VERSION 2.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB MODEL_SOURCES src/model/*.c)
file(GLOB DB_SOURCES src/kmldb/*.c)
file(GLOB CONTROLLER_SOURCES src/controler/*.c)
file(GLOB UTIL_SOURCES src/utils/*.c)

add_executable(programa
    src/main.c
    src/view/cli/main_menu.c
    ${MODEL_SOURCES}
    ${DB_SOURCES}
    ${CONTROLLER_SOURCES}
    ${UTIL_SOURCES}
)
target_include_directories(programa PRIVATE src/model src/kmldb src/controler)
if(NOT WIN32)
    target_link_libraries(programa PRIVATE m)
endif()
set_target_properties(programa PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Bin/Debug"
)

enable_testing()
add_executable(db_hash_test
    tests/db_hash_test.c
    src/kmldb/db.c
    src/utils/log.c
)
target_include_directories(db_hash_test PRIVATE src/kmldb)
add_test(NAME db_hash_test COMMAND db_hash_test)

install(TARGETS programa RUNTIME DESTINATION bin)
