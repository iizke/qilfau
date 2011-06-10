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

#Default value
if [ "$OUTPUT" == "" ]; then
OUTPUT=or.log
fi
if [ "$ITERATION" == "" ]; then
ITERATION=20
fi
if [ "$DELTA" == "" ]; then
DELTA=2
fi
if [ "$NNODES" == "" ]; then
NNODES=10
fi

PROG="$LABHOME/../Debug/netlib"
MAX_N=100
MAX_D=10

for ((j=0;j<${ITERATION};j+=1)); do
for ((delta=1;delta<$MAX_D;delta++)); do
for ((nodes=10;nodes<$MAX_N;nodes+=4)); do
	$PROG -n=$nodes -d=$delta -out_result="$OUTPUT"
	sync
	#sleep 1
done
done
done

echo Compressing data ...
rm -f $OUTPUT.analysis
for ((delta=1;delta<$MAX_D;delta++)); do
for ((nodes=10;nodes<$MAX_N;nodes+=4)); do
  fmax_seq=`cat $OUTPUT | grep "delta = $delta num_nodes = $nodes" | awk '{print $3}'`
  echo $delta $nodes `calc_mean_variance_confidence $fmax_seq` >> $OUTPUT.analysis
done
done

#drawing
cat $OUTPUT.analysis |grep "^1" > $OUTPUT.analysis.1
cat $OUTPUT.analysis |grep "^2" > $OUTPUT.analysis.2
cat $OUTPUT.analysis |grep "^3" > $OUTPUT.analysis.3
cat $OUTPUT.analysis |grep "^4" > $OUTPUT.analysis.4
cat $OUTPUT.analysis |grep "^5" > $OUTPUT.analysis.5

DATA=$OUTPUT.analysis.1:$OUTPUT.analysis.2:$OUTPUT.analysis.3:$OUTPUT.analysis.4:$OUTPUT.analysis.5
./draw-graph.sh --xcol 2 --ycol 3 --plot-type RATE --data $DATA -o plot.png

