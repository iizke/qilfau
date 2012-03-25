#!/bin/bash
# iizke
#
# sddedeus -> snapapps -> snapshot of applications
# It does not capture deeply into memory, just reopen
# or restart the applications using last time
# 
# This tool helps to continue working with the last 
# works. Currently for Ubuntu system
#
# Enjoy to be lazy.

BASEPATH=~/.sddedeus
SNAP_CONSOLE=$BASEPATH/snap-consoles
SNAP_EVINCE=$BASEPATH/snap-evinces
SNAP_VIM=$BASEPATH/snap-vims
SNAP_FIREFOX=$BASEPATH/snap-firefox
SNAP_TEXMAKER=$BASEPATH/snap-texmaker

init () {
  mkdir -p $BASEPATH
  touch $SNAP_CONSOLE
  touch $SNAP_EVINCE
  touch $SNAP_VIM
  touch $SNAP_FIREFOX
  touch $SNAP_TEXMAKER
}

snap_console () {
  # list all opened console 
  rm -f $SNAP_CONSOLE
  touch $SNAP_CONSOLE
  for f in `pgrep bash`; do
    #local dir=`ls -l /proc/$f/cwd | awk -F "-> " '{print "\"" $2 "\"" }'`
    local d=`ls -l /proc/$f/cwd | awk -F "-> " '{ print $2 }'`
    #dir=`echo \"$dir\"`
    local match=`cat $SNAP_CONSOLE | grep "$d" | wc -l`
    if [ $match -eq 0 ]; then
    ls -l /proc/$f/cwd | awk -F "-> " '{ print $2 }' >> $SNAP_CONSOLE
    fi
  done
}

restore_console () {
  local tabs=" "
  local li
  while read li; do 
    if [ "$li" != "" ]; then
    tabs="$tabs --tab --working-directory=$li "
    gnome-terminal --tab --working-directory="$li"
    fi
  done < $SNAP_CONSOLE
  echo $tabs
  #sleep 1
  #/usr/bin/gnome-terminal $tabs
  #echo $?
}

snap_evince () {
  ps -A -o args | grep ^evince | awk '{print $NF}' > $SNAP_EVINCE
}

restore_evince () {
  while read li; do
    evince $f &
  done < $SNAP_EVINCE;
}

snap_vim () {
  ps -A -o args |grep ^vim > $SNAP_VIM
}

restore_vim () {
  echo The last files opened by VIM are:
  cat $SNAP_VIM
}

snap_firefox () {
  #firefox auto restart the session when it is shutdown "accidentally"
  local check=`pgrep firefox`
  echo 0 > $SNAP_FIREFOX
  if [ "$check" != "" ]; then 
    killall -9 firefox
    echo 1 > $SNAP_FIREFOX
  fi
}

restore_firefox () {
  local check=`cat $SNAP_FIREFOX`
  echo Restoring firefox ... $check
  if [ $check -eq 1 ]; then
    echo Start firefox ...
    firefox &
  fi
}

snap() {
  snap_console
  snap_evince
  snap_vim
  snap_firefox
}

restore () {
  restore_console
  restore_evince
  restore_vim
  restore_firefox
}

#snap
#restore

cmdz=$1
echo Hello to my small program for $cmdz-ing current/last tasks ...
if [ "${cmdz}" == "snap" ]; then
  echo Do snapping ...
  snap
else
  echo Do restoring ...
  restore
fi

