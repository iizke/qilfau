#!/bin/bash

FILEDIR=$(dirname $(which $BASH_SOURCE))
LABHOME=$FILEDIR/..
# Bash shell script library functions
source $LABHOME/libs/libfuncs.sh 2>/dev/null

trap clean 2 EXIT SIGTERM

#clean () { return }

option_config_add "DESCRIPTION" \
                  "draw-graph.sh" \
                  "1" \
                  "Drawing graph from traced packet dump file" 
option_config_add "--help" \
                  "HELP" \
                  "0" \
                  "Help on draw-graph.sh"
option_config_add "-h" \
                  "HELP" \
                  "0" \
                  "Help on draw-graph.sh"
option_config_add "-o" \
                  "OUTPUT" \
                  "1" \
                  "Output file name (for example: h.png)"
option_config_add "-f" \
                  "DUMPFILES" \
                  "1" \
                  "Dump file from click or tcpdump. Can use this option many
                  times to draw some lines in graph"
option_config_add "--packet-count" \
                  "PACKETCOUNT" \
                  "0" \
                  "Insert more parameter: packet count"
option_config_add "--xrange" \
                  "XRANGE" \
                  "1" \
                  "Relative range of X axes, based time is the beginning time (for example: 0:1, or 10:100, ...)"
option_config_add "--xlabel" \
                  "XLABEL" \
                  "1" \
                  "Label of X axes."
option_config_add "--xcol" \
                   "XCOL" \
                   "1" \
                   "Column of X in dataset (dumpfile)"
option_config_add "--yrange" \
                  "YRANGE" \
                  "1" \
                  "Range of Y axes (for example: 0:1, or 10:100, ...)"
option_config_add "--ylabel" \
                  "YLABEL" \
                  "1" \
                  "Label of Y axes."
option_config_add "--ycol" \
                  "YCOL" \
                  "1" \
                  "Column of Y in dataset (dumpfile)"
option_config_add "--title" \
                  "TITLE" \
                  "1" \
                  "Title of the graph. For example: Arrival Curve of input packet"
option_config_add "--plot-type" \
                  "PLOTTYPE" \
                  "1" \
                  "Type of plotting data. It can be: RATE, COUNT (Default) or DENSITY"
option_config_add "--data" \
                  "DATA" \
                  "1" \
                  "Data for plotting"
option_config_add "--multiplot" \
                  "MULTIPLOT" \
                  "0" \
                  "Plots place separately and vertically"
option_config_add "--template" \
                  "TEMPLATE" \
                  "1" \
                  "Plot template"
option_config_add "--plot-option" \
                  "PLOTOPT" \
                  "1" \
                  "Plot option"

option_parse "$@"

if [ "$HELP" == "true" ]; then
  option_usage_print
  exit 0
fi

if [ "$XLABEL" == "" ]; then
  XLABEL="x"
fi

if [ "$XCOL" == "" ]; then
  XCOL=2
fi

if [ "$YLABEL" == "" ]; then
  YLABEL="y"
fi
 
if [ "$YCOL" == "" ]; then
  YCOL=1
fi

if [ "$OUTPUT" == "" ]; then
  OUTPUT=/dev/stdout
fi

if [ "$PLOTTYPE" == "" ]; then
  PLOTTYPE="COUNT"
fi

if [ "$MULTIPLOT" == "" ]; then
  MULTIPLOT="false"
fi

if [ "$TEMPLATE" == "" ]; then
  if [ "$MULTIPLOT" == "true" ]; then
    if [ "$PLOTTYPE" != "DENSE" ]; then
      TEMPLATE=$LABHOME/plot-template/draw-graph-multiplot.pg.template
    else
      TEMPLATE=$LABHOME/plot-template/draw-graph-mplot-dense.pg.template
    fi
  elif [ "$PLOTTYPE" == "DENSE" ]; then
    TEMPLATE=$LABHOME/plot-template/draw-graph-dense.pg.template
  else
    TEMPLATE=$LABHOME/plot-template/draw-graph.pg.template
  fi
fi

#Prepare data for plotting
#Convert dump file to human readable file (using to plot)
 
DUMPFILES=`echo $DUMPFILES | sed -e 's/:/ /g'`
DATA=`echo $DATA | sed -e 's/:/ /g'`
for f in $DUMPFILES; do
  CONVERTFILE=""
  _CONVERTFILE=""
  if [ "$PLOTTYPE" == "COUNT" ]; then
    CONVERTFILE=`basename $f`.convert.pc
    convert-click-dump.sh -f $f --packet-count -o $CONVERTFILE
  else
    _CONVERTFILE=`basename $f`.convert
    convert-click-dump.sh -f $f -o $_CONVERTFILE
    if [ "$PLOTTYPE" == "RATE" ]; then
      pt=0
      c=1 # count number of packets in a very small interval (0 division)
      CONVERTFILE=`basename $f`.convert.rate
      while read n t; do
        #if [ "$pt" == "0" ]; then 
        #  pt=$t
        #  continue
        #fi
        if [ "$t" == "$pt" ]; then
          c=$((c+1))
          continue
        fi
        freq=`echo $c $pt $t | awk '{printf "%.2f", $1/($3-$2)}'`
        echo $freq $t >> $CONVERTFILE
        pt=$t
        c=1
      done < $_CONVERTFILE
    else
      CONVERTFILE=$_CONVERTFILE
    fi
  fi
  #Aggregate Converted_dump_file to DATA
  #but remove the built-in data (future)
  DATA="$DATA $CONVERTFILE"
done

#prepare gnuplot
#using template to plot
PLOTSCRIPT=./draw-graph-`date +%s`.pg
register_tmp_file $PLOTSCRIPT
cat $TEMPLATE | \
                      sed -e s:XLABEL:${XLABEL}:g \
                          -e s:YLABEL:${YLABEL}:g \
                          -e s:XCOL:${XCOL}:g \
                          -e s:YCOL:${YCOL}:g \
                          -e s:TITLE:${TITLE}:g \
                          -e s:OUTPUT:${OUTPUT}:g > $PLOTSCRIPT

register_tmp_file /tmp/tmpfile

find_value_x () {
  local datafile=$1
  local yy=$2
  while read y x info; do
    if [ "${yy}" == "${y}" ]; then
      echo $x
      return
    fi
  done < $datafile
}

find_value_y () {
  local datafile=$1
  local xx=$2
  while read y x info; do
    if [ "${xx}" == "${x}" ]; then
      echo $y
      return
    fi
  done < $datafile
} 

find_minmin_x () {
  local minx=9999999999
  for f in $DATA; do
    local tmpx=""
    tmpx=`cat $f | head -1 | awk '{print $2}'`
    local check=`echo "$minx > $tmpx" |bc`
    if [ $check -eq 1 ]; then
      minx=$tmpx
    fi
  done
  echo $minx
}

#Update XRANGE to PLOTSCRIPT
if [ "$XRANGE" == "" ]; then
  touch /tmp/tmpfile
  #Remove "set xrange" statement
  cat $PLOTSCRIPT | sed -e 's:set xrange:#set xrange:g' > /tmp/tmpfile
  cat /tmp/tmpfile > $PLOTSCRIPT
else
  touch /tmp/tmpfile
  minminx=`find_minmin_x`
  x1=`echo $XRANGE | awk -F : '{printf "%.6f", $1}'`
  x2=`echo $XRANGE | awk -F : '{printf "%.6f", $2}'`
  #if [ `echo "$x1 == 0" | bc` -eq 1 ]; then
  x1=`echo $x1 $minminx | awk '{printf "%.6f", $1+$2}'`
  x2=`echo $x2 $minminx | awk '{printf "%.6f", $1+$2}'`
  XRANGE="$x1:$x2"
  #fi
  cat $PLOTSCRIPT | sed -e s/XRANGE/${XRANGE}/g > /tmp/tmpfile
  cat /tmp/tmpfile > $PLOTSCRIPT
fi

#Update YRANGE to PLOTSCRIPT
if [ "$YRANGE" == "" ]; then
  touch /tmp/tmpfile
  #Remove "set xrange" statement
  cat $PLOTSCRIPT | sed -e 's:set yrange:#set yrange:g' > /tmp/tmpfile
  cat /tmp/tmpfile > $PLOTSCRIPT
else
  ymin=`echo $YRANGE |awk -F : '{print $1}'`
  ymax=`echo $YRANGE |awk -F : '{print $2}'`
  
  touch /tmp/tmpfile
  register_tmp_file /tmp/tmpfile

  if [ "$PLOTTYPE" == "DENSITY" ]; then
    cat $PLOTSCRIPT | sed -e s/YRANGE/0:1.2/g > /tmp/tmpfile
  else
    cat $PLOTSCRIPT | sed -e s/YRANGE/${YRANGE}/g > /tmp/tmpfile
  fi
  cat /tmp/tmpfile > $PLOTSCRIPT

  #try to modify XRANGE
  #xmin=9999999999.9
  #xmax=0.1
  #for f in $DATA; do
  #  t=`find_value_x $f $ymin`
  #  if [ `echo "$t < $xmin" |bc` -eq 1 ]; then
  #    xmin=$t
  #  fi
  #  t=`find_value_x $f $ymax`
  #  if [ `echo "$t > $xmax" |bc` -eq 1 ]; then
  #    xmax=$t
  #  fi
  #done
  #if [ "$xmin" != "" -a "$xmax" != "" ]; then
  #  cat $PLOTSCRIPT | sed -e 's:#set xrange:set xrange:g' > /tmp/tmpfile
  #  cat /tmp/tmpfile | sed -e s/XRANGE/${xmin}:${xmax}/g > $PLOTSCRIPT
  #fi
fi

#Append line to show gnuplot what data is plotted
count=0
for f in $DATA; do
  if [ "$MULTIPLOT" == "false" ]; then
    if [ $count -eq 0 ]; then
      PLOTSTR="\"$f\" using $XCOL:$YCOL title \"`basename $f`\" $PLOTOPT\\"
    else
      PLOTSTR=",\"$f\" using $XCOL:$YCOL title \"`basename $f`\" $PLOTOPT\\"
    fi
  else
    PLOTSTR="set origin DX,DY+SY*$count; plot \"$f\" using $XCOL:$YCOL title \"`basename $f`\" $PLOTOPT;"
  fi
  count=$((count+1))
  echo $PLOTSTR >> $PLOTSCRIPT
done

#Calculate SY, NY in case of multiplot
if [ "$MULTIPLOT" == "true" ]; then
  cat $PLOTSCRIPT | sed -e s/NY=1/NY=${count}/g > /tmp/tmpfile
  cat /tmp/tmpfile | sed -e s/SY=1/SY=`echo $count | awk '{printf "%.2f", 0.9/$1}'`/g > $PLOTSCRIPT 
fi

#Do plotting
chmod +x $PLOTSCRIPT
$PLOTSCRIPT

