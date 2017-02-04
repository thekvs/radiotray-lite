# - Try to find libmagic header and library
#
# Usage of this module as follows:
#
#     find_package(LibMagic)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  LibMagic_ROOT_DIR         Set this variable to the root installation of
#                            libmagic if the module has problems finding the
#                            proper installation path.
#
# Variables defined by this module:
#
#  LIBMAGIC_FOUND              System has libmagic and magic.h
#  LIBMAGIC_LIBRARIES            The libmagic library
#  LIBMAGIC_INCLUDE_DIRS        The location of magic.h

find_path(LibMagic_ROOT_DIR
    NAMES include/magic.h
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # the static version of the library is preferred on OS X for the
    # purposes of making packages (libmagic doesn't ship w/ OS X)
    set(libmagic_names libmagic.a magic)
else ()
    set(libmagic_names magic)
endif ()

find_library(LIBMAGIC_LIBRARIES
    NAMES ${libmagic_names}
    HINTS ${LibMagic_ROOT_DIR}/lib
)

find_path(LIBMAGIC_INCLUDE_DIRS
    NAMES magic.h
    HINTS ${LibMagic_ROOT_DIR}/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibMagic DEFAULT_MSG
    LIBMAGIC_LIBRARIES
    LIBMAGIC_INCLUDE_DIRS
)

mark_as_advanced(
    LibMagic_ROOT_DIR
    LIBMAGIC_LIBRARIES
    LIBMAGIC_INCLUDE_DIRS
)
