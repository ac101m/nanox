#####################################################################################
#      Copyright 2009-2018 Barcelona Supercomputing Center                          #
#                                                                                   #
#      This file is part of the NANOS++ library.                                    #
#                                                                                   #
#      NANOS++ is free software: you can redistribute it and/or modify              #
#      it under the terms of the GNU Lesser General Public License as published by  #
#      the Free Software Foundation, either version 3 of the License, or            #
#      (at your option) any later version.                                          #
#                                                                                   #
#      NANOS++ is distributed in the hope that it will be useful,                   #
#      but WITHOUT ANY WARRANTY; without even the implied warranty of               #
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
#      GNU Lesser General Public License for more details.                          #
#                                                                                   #
#      You should have received a copy of the GNU Lesser General Public License     #
#      along with NANOS++.  If not, see <https://www.gnu.org/licenses/>.            #
#####################################################################################

#
# SYNOPSIS
#
#   AX_CHECK_MIC
#
# DESCRIPTION
#
#   Checks that selected compilers are able to build binaries for Intel K1OM
#   architecture.
#   Checks are limited to check that the compiler used has the proper triplet set
#   or that an Intel compiler is being used.
#   In the case of the linker (LD), a GNU ld for this specific architecture is
#   required.
#   Allowed host triplets: x86_64-k1om-linux k1om-mpss-linux
#

AC_DEFUN([AX_CHECK_MIC],
[
AC_REQUIRE([AC_PROG_CXX])
AC_BEFORE([AX_COMPILE_VENDOR],[$0])dnl this macro uses info generated by this macro

  AS_IF([test "$cross_compiling" = no],
    [AC_MSG_ERROR([
-------------------------------
Cross compilation for Intel KNC detected, but
cross compile mode was not enabled by autoconf.
Check that a cross compiler is actually being
used @{:@i.e. use $host-gcc/g++@:}@ or
compile with Intel's icc/icpc using -mmic flag.
Recommended:
CC='icc -mmic' CXX='icpc -mmic'
LD=x86_64-k1om-linux-ld
-------------------------------])
    ])

  AS_CASE([$CC],
    [*x86_64-k1om-linux*],
      [supported=yes],
    [*k1om-mpss-linux*],
      [supported=yes],
    [
      AS_IF([test "$ax_cv_c_compiler_vendor" = intel],
        [supported=yes],
        [supported=no])
    ])

  AS_IF([test "$supported" = yes],[
    AS_CASE([$CXX],
      [*x86_64-k1om-linux*],
        [supported=yes],
      [*k1om-mpss-linux*],
        [supported=yes],
      [
        AS_IF([test "$ax_cv_cxx_compiler_vendor" = intel],
          [supported=yes],
          [supported=no])
      ])
  ])

  AS_IF([test "$supported" = yes],[
    AS_CASE([$LD],
      [*"x86_64-k1om-linux-ld "*],
        [supported=yes],
      [*"k1om-mpss-linux-ld "*],
        [supported=yes],
      [
        # Default
        supported=no
      ])
  ])

  AS_IF([test "$supported" = no],
    [AC_MSG_ERROR([
-------------------------------
Cross compilation for Intel KNC detected, but
$stage support check failed.
Please, select either a suitable cross-compiler
@{:@e.g.: $host-gcc@:}@ or Intel's icc.
Recommended:
CC='icc -mmic' CXX='icpc -mmic'
LD=x86_64-k1om-linux-ld
-------------------------------])
     ])
])dnl AX_CHECK_MIC

