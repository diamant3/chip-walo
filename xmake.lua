set_project("chip-walo")
set_languages("clatest", "cxxlatest")
set_optimize("fastest")
set_warnings("everything")

add_requires("libsdl")

target("chip-walo")
    add_packages("libsdl")
    set_kind("binary")
    add_files("src/*.c")