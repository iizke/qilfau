#!/bin/bash

FILEDIR=$(dirname $(which $BASH_SOURCE))
LABHOME=$FILEDIR/..
# Bash shell script library functions
source $LABHOME/libs/libfuncs.sh 2>/dev/null

trap clean 2 EXIT SIGTERM

#clean () { return }

option_config_add "DESCRIPTION" \
                  "exp-graph.sh" \
                  "1" \
                  "Experiment with graph in operation research" 
option_config_add "--help" \
                  "HELP" \
                  "0" \
                  "Help on exp-graph.sh"
option_config_add "-h" \
                  "HELP" \
                  "0" \
                  "Help on exp-graph.sh"
option_config_add "-o" \
                  "OUTPUT" \
                  "1" \
                  "Output file name (for example: h.png)"
option_config_add "-d" \
                  "DELTA" \
                  "1" \
                  "Number of Tx and Rx per nodes"
option_config_add "-n" \
                  "NNODES" \
                  "1" \
                  "Number of nodes"
option_config_add "-i" \
                  "ITERATION" \
                  "1" \
                  "Number of iterations"

option_parse "$@"

PROG="$LABHOME/../Debug/netlib"
for ((j=0;j<${ITERATION};j+=1)); do
	$PROG -n=$NNODES -d=$DELTA -out_result="$OUTPUT"
	sync
	sleep 1
done
