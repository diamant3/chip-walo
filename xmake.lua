set_project("chip-walo")
set_languages("clatest")
set_optimize("fastest")
set_warnings("everything")

add_packages("libsdl")
add_requires("libsdl")

target("chip-walo")
    set_kind("binary")
    add_files("sources/*.c")