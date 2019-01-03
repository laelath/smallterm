find_package (PkgConfig)

pkg_check_modules (MINITERM_LIBS REQUIRED vte-2.91 glib-2.0)

include_directories (${MINITERM_LIBS_INCLUDE_DIRS})
link_directories (${MINITERM_LIBS_LIBRARY_DIRS})

set (SOURCES miniterm.c settings.c terminal.c)
add_executable (miniterm ${SOURCES})
target_link_libraries (miniterm ${MINITERM_LIBS_LIBRARIES})

install (TARGETS miniterm DESTINATION bin)
