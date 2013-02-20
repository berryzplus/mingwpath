dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([mingwpath], [0.10], [berryzplus@gmail.com])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/mingwpath.c])

AM_INIT_AUTOMAKE()

# Checks for programs.
AC_PROG_CC
AC_C_INLINE

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_ARG_WITH(unicode,
  AS_HELP_STRING([--with-unicode], [use unicode (default is YES)]),
  [ac_cv_with_unicode=$withval], [ac_cv_with_unicode=yes])
if test $ac_cv_with_unicode = yes; then
  AC_DEFINE(_UNICODE, 1, [Define to 1 if you use unicode.])
fi

AH_BOTTOM([

#if _UNICODE
#define UNICODE 1
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <tchar.h>

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT_WINXP  0x0501
#define _WIN32_WINNT        _WIN32_WINNT_WINXP
#define WINVER              _WIN32_WINNT
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifndef _SHLOBJ_H
#include <shlobj.h>
#endif


])
AC_CONFIG_FILES(Makefile)
AC_OUTPUT
