set_project("chip-walo")
set_languages("clatest", "cxxlatest")
-- set_optimize("fastest")

add_packages("libsdl")
add_requires("libsdl")

target("chip-walo")
    set_kind("binary")
    add_files("src/*.c")