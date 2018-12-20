find_package (PkgConfig)

set (SOURCES miniterm.c settings.c)
add_executable (miniterm ${SOURCES})

pkg_check_modules (MINITERM_LIBS REQUIRED vte-2.91 glib-2.0)

include_directories (${MINITERM_LIBS_INCLUDE_DIRS})
target_link_libraries (miniterm ${MINITERM_LIBS_LIBRARIES})

install (TARGETS miniterm DESTINATION bin)
