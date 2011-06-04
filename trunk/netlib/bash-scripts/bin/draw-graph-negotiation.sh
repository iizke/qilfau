#!/bin/bash

FILEDIR=$(dirname $(which $BASH_SOURCE))
LABHOME=$FILEDIR/..
# Bash shell script library functions
source $FILEDIR/../libs/libfuncs.sh 2>/dev/null

trap clean 2 EXIT SIGTERM

#clean () { return }

option_config_add "DESCRIPTION" \
                  "draw-graph-negotiation.sh" \
                  "1" \
                  "Drawing graph from traced packet dump file, especially for frame relay network" 
option_config_add "--help" \
                  "HELP" \
                  "0" \
                  "Help on draw-graph-negotiation.sh"
option_config_add "-h" \
                  "HELP" \
                  "0" \
                  "Help on draw-graph-negotiation.sh"
option_config_add "-o" \
                  "OUTPUT" \
                  "1" \
                  "Output file name (for example: h.png)"
option_config_add "-f" \
                  "DUMPFILES" \
                  "1" \
                  "Dump file from click or tcpdump. Can use this option many times to draw some lines in graph"
option_config_add "--data" \
                  "DATA" \
                  "1" \
                  "Data for plotting"
option_config_add "--cir" \
                  "CIR" \
                  "1" \
                  "Committed Information Rate (packet per second)"
option_config_add "--cbs" \
                  "CBS" \
                  "1" \
                  "Committed Burst Size (packets)"
option_config_add "--ebs" \
                  "EBS" \
                  "1" \
                  "Excess Burst Size (packets)"
option_config_add "--access-rate" \
                  "ACR" \
                  "1" \
                  "Access Rate (packet per second)"
option_config_add "--base" \
                  "BASETIME" \
                  "1" \
                  "Base time, from this we plot"
option_parse "$@"

# Check parameters
if [ "$HELP" == "true" ]; then
  option_usage_print
  exit 0
fi

if [ "$OUTPUT" == "" ]; then
  OUTPUT=/dev/stdout
fi
check=`is_number $CIR`
if [ "$check" == "false" ]; then
  CIR=0
fi
check=`is_number $CBS`
if [ "$check" == "false" ]; then
  CBS=0
fi

check=`is_number $EBS`
if [ "$check" == "false" ]; then
  EBS=0
fi

check=`is_number $ACR`
if [ "$check" == "false" ]; then
  ACR=0
fi

if [ "$BASETIME" == "" ]; then
  BASETIME=0
fi

# Calculate T
get_window_time () {
  #local cir=$1
  #local cbs=$2
  #local ebs=$3
  #local acr=$4
  local T=0
  if [ $CBS -gt 0 -a $CIR -gt 0 ]; then
    T=`echo $CBS $CIR | awk '{print $1/$2}'`
  elif [ $ACR -gt 0 ]; then
    T=`echo $EBS $ACR | awk '{print $1/$2}'`
  fi
  echo $T
}

WINDOW_TIME=`get_window_time`
# Unit test: get_window_time
#get_window_time
ENDTIME=`echo $BASETIME $WINDOW_TIME | awk '{printf "%.16f", $1+$2}'`
XRANGE=$BASETIME:$ENDTIME
#Build the data option
DATAOPT=""
if [ "$DUMPFILES" != "" ]; then
  DATAOPT="-f $DUMPFILES "
fi
if [ "$DATA" != "" ]; then
  DATAOPT="$DATAOPT --data $DATA"
fi

#prepare CBS file
touch ./CBS_line
register_tmp_file ./CBS_line
echo "$CBS $BASETIME" > ./CBS_line
echo "$CBS $ENDTIME" >> ./CBS_line

#prepare CIR file
touch ./CIR_line
register_tmp_file ./CIR_line
echo "0 $ENDTIME" > ./CIR_line
echo "$((CBS+EBS+1)) $ENDTIME" >> ./CIR_line

#prepare EBS file
touch ./EBS_line
register_tmp_file ./EBS_line
echo "$((CBS+EBS)) $BASETIME" > ./EBS_line
echo "$((CBS+EBS)) $ENDTIME" >> ./EBS_line

draw-graph.sh \
        $DATAOPT \
        --plot-type "COUNT" \
        --xrange $XRANGE \
        -o $OUTPUT \
        --xlabel "time" \
        --ylabel "packets" \
        --title "Conformant_verification" 
