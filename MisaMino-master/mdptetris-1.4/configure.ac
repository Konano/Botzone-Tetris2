#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([mdptetris], [1.4], [christophe.thiery@loria.fr])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/tetris.c])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([m], [main], , AC_MSG_ERROR([The mathematic library (libm) is required]))
AC_CHECK_LIB([gslcblas], [main], , AC_MSG_ERROR([The GNU Scientific Library (libgsl) is required]))
AC_CHECK_LIB([gsl], [main], , AC_MSG_ERROR([The GNU Scientific Library (libgsl) is required]))
AC_CHECK_LIB([X11], [main], , AC_MSG_ERROR([The X11 library is required]))
AC_CHECK_LIB([GL], [main], , AC_MSG_ERROR([The GL library is required]))
AC_CHECK_LIB([GLU], [main], , AC_MSG_ERROR([The GLU library is required]))
AC_CHECK_LIB([glut], [main], , AC_MSG_ERROR([The glut library is required]))
AC_CHECK_LIB([z], [main], , AC_MSG_ERROR([The zlib library is required]))

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdint.h stdlib.h string.h signal.h X11/Xlib.h X11/Xutil.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_CHECK_FUNCS([pow sqrt])

AC_CONFIG_FILES(Makefile src/Makefile data/Makefile rlc2008agent/Makefile)

AC_OUTPUT

