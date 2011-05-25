#!/bin/csh
#  ranlib_prb1.csh  22 August 2000
#
setenv DEBUG TRUE
set echo
#
if ( $DEBUG == TRUE ) then
  cc -c -g ranlib_prb1.c
  if ( $status != 0 ) exit
else
  cc -c ranlib_prb1.c
  if ( $status != 0 ) exit
endif
#
cc ranlib_prb1.o -L$HOME/lib/$ARCH -lranlib -lm
if ( $status != 0 ) exit
#
rm ranlib_prb1.o
mv a.out ranlib_prb1
#
ranlib_prb1 > ranlib_prb1.out
#
if ( $status == 0 ) then
  rm ranlib_prb1
endif
#
echo "Normal end of ranlib_prb1 tests."
