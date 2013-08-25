#!/bin/bash

FILEDIR=$(dirname $(which $BASH_SOURCE))
LABHOME=$FILEDIR/..
# Bash shell script library functions
source $LABHOME/libs/libfuncs.sh 2>/dev/null

trap clean 2 EXIT SIGTERM

#clean () { return }

option_config_add "DESCRIPTION" \
                  "test-babs.sh" \
                  "1" \
                  "running script of testing BABS queue" 
option_config_add "--help" \
                  "HELP" \
                  "0" \
                  "Help on test-babs.sh"
option_config_add "-h" \
                  "HELP" \
                  "0" \
                  "Help on test-babs.sh"
option_config_add "-t" \
                  "TEMPLATE_FILE" \
                  "1" \
                  "Template file of configuration"
option_config_add "-o" \
                  "OUTPUT" \
                  "1" \
                  "Output file"

option_parse "$@"

#Default value
if [ "$TEMPLATE_FILE" == "" ]; then
TEMPLATE_FILE="src/netsim/conf/test_babs_template.conf"
fi
if [ "$OUTPUT" == "" ]; then
OUTPUT="output"
fi


PROG="$LABHOME/../Debug/netlib"
####

QUEUE_TYPE="QUEUE_BURST_FIFO QUEUE_BURST_SCHED1 QUEUE_BURST_SCHEDWIN"
BURST_MEAN="10 30 50 70 90"
BURST_SDEV="10 20 30 40 50"
ARRIVAL_RATE="0.3 0.5 0.7 1.0 1.1 1.2 1.3 1.4 1.5 1.6 1.7 1.8 1.9 2.0"
WINDOW="8 10 12 14 0"

TEMP_CONF="/tmp/babs_temp.conf"
rm -f $TEMP_CONF
for qt in $QUEUE_TYPE; do
for ar in $ARRIVAL_RATE; do
for bm in $BURST_MEAN; do
for bs in $BURST_SDEV; do
  echo "arrival.lambda = $ar" >> $TEMP_CONF
  if [ "$qt" == "QUEUE_BURST_FIFO" ]; then
    rm -f $TEMP_CONF
    cp $TEMPLATE_FILE $TEMP_CONF
    echo "arrival.lambda = $ar;" >> $TEMP_CONF
    echo "burst.mean = $bm;" >> $TEMP_CONF
    echo "burst.sdev = $bs;" >> $TEMP_CONF
    echo "queue.type = $qt;" >> $TEMP_CONF
    echo "Testing $qt - ar $ar - burst $bm $bs"
    $PROG $TEMP_CONF >> $OUTPUT
  else
    for w in $WINDOW; do
      if [ "$w" == "0" -a "$qt" == "QUEUE_BURST_SCHEDWIN" ]; then
      continue
      fi 
      rm -f $TEMP_CONF
      cp $TEMPLATE_FILE $TEMP_CONF
      echo "arrival.lambda = $ar;" >> $TEMP_CONF
      echo "burst.mean = $bm;" >> $TEMP_CONF
      echo "burst.sdev = $bs;" >> $TEMP_CONF
      echo "queue.type = $qt;" >> $TEMP_CONF
      echo "queue.window = $w;" >> $TEMP_CONF
      echo "Testing $qt - ar $ar - burst $bm $bs - window $w"
      $PROG $TEMP_CONF >> $OUTPUT
    done
  fi

done
done
done
done

#$PROG
