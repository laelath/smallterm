cmake_minimum_required (VERSION 3.5.2)

project (miniterm)

# Generate compile_commands.json for integration into completion engines.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory (src)

install (FILES miniterm.desktop DESTINATION share/applications)
