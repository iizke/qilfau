#!/bin/csh
#  ranlib.csh  23 August 2000
#
setenv DEBUG TRUE
set echo
#
if ( $DEBUG == TRUE ) then
  cc -c -g com.c
  if ( $status != 0 ) exit
  cc -c -g linpack.c
  if ( $status != 0 ) exit
  cc -c -g ranlib.c
  if ( $status != 0 ) exit
else
  cc -c com.c
  if ( $status != 0 ) exit
  cc -c linpack.c
  if ( $status != 0 ) exit
  cc -c ranlib.c
  if ( $status != 0 ) exit
endif
#
ar qc libranlib.a *.o
rm *.o
#
mv libranlib.a ~/lib/$ARCH
#
echo "A new version of ranlib has been created."
