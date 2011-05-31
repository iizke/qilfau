#!/bin/bash

FILEDIR=$(dirname $(which $BASH_SOURCE))
# Bash shell script library functions
source $FILEDIR/../libs/libfuncs.sh 2>/dev/null
FILEDIR=$(get_abs_path "$FILEDIR/../.")
clean () {
  killall -9 click 2>/dev/null
  killall -9 clicky 2>/dev/null
}

trap clean 2 EXIT SIGTERM

usage () {
  echo -e "\

  ${bold}DESCRIPTION${txtrst}
    Initialize enviroment for NETLIB

    ${bold}SYNOPSIS${txtrst}
      ${txtylw}${bold}init.sh  [${txtylw}${bold}-h | --help${txtrst}]"
}

option_config_add "-h"            "HELP"      "0" "Help on init"
option_config_add "--help"        "HELP"      "0" "Help on init"
option_parse "$@"

if [ "$HELP" == "true" ]; then
  usage
  exit 0
fi

#Setting environment variables
  #Setting PATH for visual-click
  EXPSTR="export PATH=\$PATH:$FILEDIR/bin"
  count=`cat ~/.bashrc|grep -e "export PATH=\\\$PATH:$FILEDIR/bin"|wc -l`
  if [ $count -eq 0 ]; then
    echo $EXPSTR >> ~/.bashrc
  fi
  #Setting .clickrc for click
  #count=`cat ~/.bashrc|grep "source ~/.clickrc" | wc -l`
  #EXPSTR="source ~/.clickrc"
  #if [ $count -eq 0 ]; then
  #  echo $EXPSTR >> ~/.bashrc
  #fi


#Finish
print_warn "Now, you need to open and work in a new console to use new
environment supporting NETLIB. Good luck."
