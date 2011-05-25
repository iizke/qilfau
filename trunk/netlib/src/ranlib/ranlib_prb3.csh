#!/bin/csh
#  ranlib_prb3.csh  22 August 2000
#
setenv DEBUG TRUE
set echo
#
if ( $DEBUG == TRUE ) then
  cc -c -g ranlib_prb3.c
  if ( $status != 0 ) exit
else
  cc -c ranlib_prb3.c
  if ( $status != 0 ) exit
endif
#
cc ranlib_prb3.o -L$HOME/lib/$ARCH -lranlib -lm
if ( $status != 0 ) exit
#
rm ranlib_prb3.o
mv a.out ranlib_prb3
#
ranlib_prb3 > ranlib_prb3.out
#
if ( $status == 0 ) then
  rm ranlib_prb3
endif
#
echo "Normal end of ranlib_prb3 tests."
