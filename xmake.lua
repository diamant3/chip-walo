set_project("chip-walo")
set_languages("c11", "c++11")

add_requires("libsdl")

target("chip-walo")
    set_kind("binary")
    add_files("src/*.c")
    add_packages("libsdl")
target_end()