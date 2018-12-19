cmake_minimum_required (VERSION 2.8)

project (miniterm)

add_subdirectory (src)

install (FILES miniterm.desktop DESTINATION share/applications)
