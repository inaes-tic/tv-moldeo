e -# initial information about the project
AC_INIT([Moldeo],[0.7],[contact@moldeo.coop])

# check if the source folder is available
AC_CONFIG_SRCDIR([moldeo/moActions.cpp])

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# automake initialization (mandatory) including a check for automake API version >= 1.11
AM_INIT_AUTOMAKE([1.11 -Wall no-define -Wno-portability tar-ustar no-dist-gzip dist-xz])
AM_MAINTAINER_MODE([enable])
AC_CONFIG_MACRO_DIR([m4])

# use the C++ compiler for the following checks
AC_LANG([C++])

# Version
MOLDEO_MAJOR_VERSION=0
MOLDEO_MINOR_VERSION=7
MOLDEO_MICRO_VERSION=3
MOLDEO_VERSION=$MOLDEO_MAJOR_VERSION.$MOLDEO_MINOR_VERSION.$MOLDEO_MICRO_VERSION

MOLDEO_LT_CURRENT=1
MOLDEO_LT_REVISION=0
MOLDEO_LT_AGE=0

AC_SUBST(MOLDEO_VERSION)
AC_SUBST(MOLDEO_LT_CURRENT)
AC_SUBST(MOLDEO_LT_REVISION)
AC_SUBST(MOLDEO_LT_AGE)

# required versions of other packages
m4_define([sdl_required_version], [1.2])
m4_define([gl_required_version],  [5.0])
m4_define([lua_required_version], [5.1])

m4_define([freetype_required_version],  [14.0])
m4_define([tinyxml_required_version],   [2.6])

SDL_REQUIRED_VERSION=sdl_required_version
GL_REQUIRED_VERSION=gl_required_version

LUA_REQUIRED_VERSION=lua_requierd_version
FREETYPE_REQUIRED_VERSION=freetype_requierd_version
TINYXML_REQUIRED_VERSION=tinyxml_requierd_version


AC_SUBST(GTK_REQUIRED_VERSION)
AC_SUBST(INTROSPECTION_REQUIRED_VERSION)

PKG_CHECK_MODULES(SDL,
  [sdl >= sdl_required_version])
PKG_CHECK_MODULES(SDL_IMAGE,
  [SDL_image >= sdl_required_version])
PKG_CHECK_MODULES(SDL_NET,
  [SDL_net >= sdl_required_version])

PKG_CHECK_MODULES(GL,
  [gl >= gl_required_version])
PKG_CHECK_MODULES(GLU,
  [glu >= gl_required_version])

PKG_CHECK_MODULES(LUA,
  [lua5.1 >= lua_required_version])

PKG_CHECK_MODULES(FREETYPE,
  [freetype2 >= freetype_required_version])

PKG_CHECK_MODULES(TINYXML,
  [tinyxml >= tinyxml_required_version])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

dnl Initialize libtool
LT_PREREQ([2.2.6])
LT_INIT([disable-static win32-dll])
# Create libtool early, because it's used in configure
LT_OUTPUT

# distribute additional compiler and linker flags among Makefiles
# --> set and change these variables instead of CXXFLAGS or LDFLAGS (for user only)
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_LDFLAGS])

# files to generate via autotools (prepare .am or .in source files)
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([moldeo/Makefile
                 moldeo.pc])
#                 src/Makefile
#		 plugins/Makefile])

# finally this generates the Makefiles etc. for the build
AC_OUTPUT
