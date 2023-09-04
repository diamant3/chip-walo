cmake_minimum_required(VERSION 3.5)
project(chip-walo)

enable_language(C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED On)
set(CMAKE_C_EXTENSIONS Off)

find_package(SDL2 REQUIRED)

add_executable(${PROJECT_NAME}
    sources/main.c
    sources/peripherals.c
    sources/chip_walo.c
    sources/opcodes.c
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

if(UNIX)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
        m
    )
endif()