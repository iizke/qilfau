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
                  "MAX_DELTA" \
                  "1" \
                  "Number of Tx and Rx per nodes"
option_config_add "-n" \
                  "MAX_NODES" \
                  "1" \
                  "Number of nodes"
option_config_add "-i" \
                  "ITERATION" \
                  "1" \
                  "Number of iterations"

option_parse "$@"

#Default value
if [ "$OUTPUT" == "" ]; then
OUTPUT=fmax
fi
if [ "$ITERATION" == "" ]; then
ITERATION=10
fi
if [ "$MAX_DELTA" == "" ]; then
MAX_DELTA=80
fi
if [ "$MAX_NODES" == "" ]; then
MAX_NODES=70
fi

#PROG="$LABHOME/../Debug/netlib-poligraph"
#PROG="$LABHOME/../Debug/netlib-greedy"
PROG="$LABHOME/../Debug/netlib-rwa-prob"
#PROG="$LABHOME/../Debug/netlib-rwa-ring"
MAX_N=$MAX_NODES
MAX_D=$MAX_DELTA
OUTPUT=lmax.rwa
XLABEL="node"
YLABEL="fmax"
nodes=
delta=
j=0
##########
run_this=0
if [ "$run_this" == "1" ]; then
rm -f $OUTPUT
for ((j=0;j<${ITERATION};j++)); do
#for ((delta=1;delta<=${MAX_D};delta++)); do
for ((delta=10;delta<=${MAX_D};delta+=10)); do
for ((nodes=10;nodes<=${MAX_N};nodes+=10)); do
	#echo RUNNNING ...
  #$PROG -n=$nodes -d=$delta -out_result="$OUTPUT" -seed=$((j+50))
  $PROG -n=$nodes -req=$delta -out_result="$OUTPUT" -seed=$((j+50)) -prob=0.3
	sync
	#sleep 1
done
done
done
fi

##########
run_this=0
if [ "$run_this" == "1" ]; then
echo Compressing data ...
rm -f $OUTPUT.analysis
for ((delta=10;delta<=$MAX_D;delta+=10)); do
for ((nodes=10;nodes<=$MAX_N;nodes+=10)); do
  fmax_seq=`cat $OUTPUT | grep "delta = $delta num_nodes = $nodes" | awk '{print $3}'`
  echo $delta $nodes `calc_mean_variance_confidence $fmax_seq` >> $OUTPUT.analysis
done
done
fi

cat $OUTPUT.analysis | sed -e 's/nan/0/g' > t
mv t $OUTPUT.analysis


#drawing
#get data oriented to number-of node v.s max-traffic (fix number of tx/rx)
cat $OUTPUT.analysis |grep "^10" > $OUTPUT.analysis.r1
cat $OUTPUT.analysis |grep "^20" > $OUTPUT.analysis.r2
cat $OUTPUT.analysis |grep "^30" > $OUTPUT.analysis.r3
cat $OUTPUT.analysis |grep "^40" > $OUTPUT.analysis.r4
cat $OUTPUT.analysis |grep "^50" > $OUTPUT.analysis.r5
cat $OUTPUT.analysis |grep "^60" > $OUTPUT.analysis.r6
cat $OUTPUT.analysis |grep "^70" > $OUTPUT.analysis.r7
cat $OUTPUT.analysis |grep "^80" > $OUTPUT.analysis.r8


#DATA=$OUTPUT.analysis.d1:$OUTPUT.analysis.d2:$OUTPUT.analysis.d3:$OUTPUT.analysis.d4:$OUTPUT.analysis.d5
DATA=$OUTPUT.analysis.r1:$OUTPUT.analysis.r2:$OUTPUT.analysis.r3:$OUTPUT.analysis.r4:$OUTPUT.analysis.r5:$OUTPUT.analysis.r6:$OUTPUT.analysis.r7
XLABEL="nodes"
./draw-graph.sh --xcol 2 --ycol 3 --yerrorbar 4 --xlabel "$XLABEL" --ylabel "$YLABEL" --plot-type RATE --data $DATA -o plot_rwa_n_l.png

#get data oriented to number-of tx/rx v.s max-traffic (fix number of node)
cat $OUTPUT.analysis |awk '{if ($2 == 10) print $0}' > $OUTPUT.analysis.n10
cat $OUTPUT.analysis |awk '{if ($2 == 20) print $0}' > $OUTPUT.analysis.n20
cat $OUTPUT.analysis |awk '{if ($2 == 30) print $0}' > $OUTPUT.analysis.n30
cat $OUTPUT.analysis |awk '{if ($2 == 40) print $0}' > $OUTPUT.analysis.n40
cat $OUTPUT.analysis |awk '{if ($2 == 50) print $0}' > $OUTPUT.analysis.n50
cat $OUTPUT.analysis |awk '{if ($2 == 60) print $0}' > $OUTPUT.analysis.n60
cat $OUTPUT.analysis |awk '{if ($2 == 70) print $0}' > $OUTPUT.analysis.n70
cat $OUTPUT.analysis |awk '{if ($2 == 80) print $0}' > $OUTPUT.analysis.n80
#cat $OUTPUT.analysis |grep "^[2-9] 90" > $OUTPUT.analysis.n90

DATA=$OUTPUT.analysis.n10:$OUTPUT.analysis.n20:$OUTPUT.analysis.n30:$OUTPUT.analysis.n40:$OUTPUT.analysis.n50:$OUTPUT.analysis.n60:$OUTPUT.analysis.n70
#:$OUTPUT.analysis.n80:$OUTPUT.analysis.n90
XLABEL="requests"
./draw-graph.sh --xcol 1 --ycol 3 --yerrorbar 4 --xlabel "$XLABEL" --ylabel "$YLABEL" --plot-type RATE --data $DATA -o plot_rwa_r_l.png

#drawing
#seed v/s fmax

#cat $OUTPUT | grep "delta = 2 num_nodes = 50" > $OUTPUT.analysis.s
#DATA=$OUTPUT.analysis.s
#./draw-graph.sh --xcol 12 --ycol 3 --xlabel "seed" --ylabel "fmax" --plot-type RATE --data $DATA -o plot_s_f.png
numnodes=20
delta=5
pngfile=plot_s_f${delta}.png
outfile="$OUTPUT".seed.n20.d5
seedmax=70
loop=5
run_this=0
if [ "$run_this" == "1" ]; then
  rm "$outfile"
  for ((j=0;j<$loop;j++)); do
  for ((s=10;s<=$seedmax;s++)); do
    $PROG -n=$numnodes -d=$delta -in_trf="trf_Mbps.dat" -out_result="$outfile" -seed=$s
    sync
  done
  done
fi

run_this=0
if [ "$run_this" == "1" ]; then
  rm -f "$outfile".analysis
  for ((s=10;s<=$seedmax;s++)); do
    fmax_seq=`cat $outfile | grep "seed = $s" | awk '{print $3}'`
    echo $s `calc_mean_variance_confidence $fmax_seq` >> $outfile.analysis
  done
fi

cat $outfile.analysis | sed -e 's/nan/0/g' -e 's/-0/0/g' > t
mv t $outfile.analysis

DATA=$outfile.analysis
./draw-graph.sh --xcol 1 --ycol 2 --yerrorbar 3 --xlabel "seed" --ylabel "fmax" --plot-type RATE --data $DATA -o $pngfile

# Drawing prob
##########
run_this=0
if [ "$run_this" == "1" ]; then
#rm -f "$OUTPUT".prob.n40 "$OUTPUT".prob.n20
for ((j=0;j<${ITERATION};j++)); do
for ((p=1;p<=10;p++)); do
  pb=`echo $p | awk '{print 0.1 * $1}'`
  for ((s=50;s<=60;s++)); do
    $PROG -n=40 -out_result="$OUTPUT".prob.n40 -seed=$s -prob=$pb -req=40
    $PROG -n=20 -out_result="$OUTPUT".prob.n20 -seed=$s -prob=$pb -req=40
    $PROG -n=60 -out_result="$OUTPUT".prob.n60 -seed=$s -prob=$pb -req=40
    sync
  done
done
done
fi

run_this=1
if [ "$run_this" == "1" ]; then
  rm -f $OUTPUT.prob.analysis.*
  for ((p=1;p<10;p++)); do
    pb=`echo $p | awk '{print 0.1 * $1}'`
    fmax_seq=`cat $OUTPUT.prob.n40 | grep "prob = $pb" | awk '{if ($3 > 0) print $3}'`
    echo $pb `calc_mean_variance_confidence $fmax_seq` >> $OUTPUT.prob.analysis.n40
    fmax_seq=`cat $OUTPUT.prob.n20 | grep "prob = $pb" | awk '{if ($3 > 0) print $3}'`
    echo $pb `calc_mean_variance_confidence $fmax_seq` >> $OUTPUT.prob.analysis.n20
    fmax_seq=`cat $OUTPUT.prob.n60 | grep "prob = $pb" | awk '{if ($3 > 0) print $3}'`
    echo $pb `calc_mean_variance_confidence $fmax_seq` >> $OUTPUT.prob.analysis.n60
          
  done
fi

cat $OUTPUT.prob.analysis.n40 | sed -e 's/nan/0/g' -e 's/-0/0/g' > t
mv t $OUTPUT.prob.analysis.n40

cat $OUTPUT.prob.analysis.n20 | sed -e 's/nan/0/g' -e 's/-0/0/g' > t
mv t $OUTPUT.prob.analysis.n20

cat $OUTPUT.prob.analysis.n60 | sed -e 's/nan/0/g' -e 's/-0/0/g' > t
mv t $OUTPUT.prob.analysis.n60

#DATA=$OUTPUT.prob.n60:$OUTPUT.prob.n40:$OUTPUT.prob.n20
#./draw-graph.sh --xcol 15 --ycol 3 --xlabel "prob" --ylabel "fmax" --plot-type RATE --data $DATA -o plot_pp_f.png --template /home/iizke/projects/netlib/bash-scripts/plot-template/draw-graph-dense.pg.template
DATA=$OUTPUT.prob.analysis.n60:$OUTPUT.prob.analysis.n40:$OUTPUT.prob.analysis.n20
./draw-graph.sh --xcol 1 --ycol 2 --yerrorbar 3 --xlabel "prob" --ylabel "max_lambda" --plot-type RATE --data $DATA -o plot_rwa_pl_f.png

####
