set_project("chip-walo")
set_languages("ansi")
set_optimize("fastest")

add_requires("libsdl")

target("chip-walo")
    add_packages("libsdl")
    set_kind("binary")
    add_files("src/*.c")