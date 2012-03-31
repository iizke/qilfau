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
SNAP_DIRS=$BASEPATH/snap-dirs

get_abs_path () {
  local _p=$1
  local _curdir=`pwd`
  if [ -e "$_p" ]; then
    local _basen=`basename $_p`
    local _dirn=`dirname $_p`
    cd $_dirn
    if [ $? -ne 0 ]; then
      return
    fi
    if [ "$_basen" == "." ]; then
      echo `pwd`
    else
      echo `pwd`/$_basen
    fi
    cd $_curdir
  fi
}

init () {
  mkdir -p $BASEPATH
  touch $SNAP_CONSOLE
  touch $SNAP_EVINCE
  touch $SNAP_VIM
  touch $SNAP_FIREFOX
  touch $SNAP_TEXMAKER
  touch $SNAP_DIRS
}

snap_console () {
  # list all opened console 
  echo -------------------------------------------------------------------------
  echo Snapping opened consoles ...
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
  echo Snapped consoles are:
  cat $SNAP_CONSOLE
}

restore_console () {
  echo -------------------------------------------------------------------------
  echo Restoring last opened consoles ...
  echo They are: ...
  cat $SNAP_CONSOLE

  local tabs=" "
  local li
  while read li; do
    if [ "$li" != "" ]; then
    tabs="$tabs --tab --working-directory=$li "
    #gnome-terminal --tab --working-directory="$li"
    fi
  done < $SNAP_CONSOLE
  #echo $tabs
  #sleep 1
  gnome-terminal $tabs
  #echo $?
}

snap_evince () {
  echo -------------------------------------------------------------------------
  echo Snapping opened PDF files by evince ...
  ps -A -o args | grep ^evince | awk '{print $NF}' > $SNAP_EVINCE
  echo The opened pdf files are:
  cat $SNAP_EVINCE
  # if there are some opened pdf files then these directories containing them
  # also be opened -> call nautilus or pcmanfm to open them when restoring 
  # step starts 
  snap_dirs
}

snap_dirs () {
  echo -------------------------------------------------------------------------
  echo Snapping guessed opened directories derived from pdf files ...
  rm -f $SNAP_DIRS
  touch $SNAP_DIRS
  while read li; do
    # check existing dirname in SNAP_DIRS
    local d=`dirname "$li"`
    local check=`cat $SNAP_DIRS | grep $d | wc -l`
    if [ $check -eq 0 ]; then
      echo "$d" >> $SNAP_DIRS
    fi
  done < $SNAP_EVINCE
  echo The opened dirs could be:
  cat $SNAP_DIRS
}

restore_evince () {
  echo -------------------------------------------------------------------------
  echo Restoring last opened PDF files ...
  while read li; do
    evince "$li" 2>/dev/null &
  done < $SNAP_EVINCE;
}

snap_vim () {
  echo -------------------------------------------------------------------------
  echo Snapping files opened by VIM ...
  ps -A -o args |grep ^vim > $SNAP_VIM
  echo You should finish editing these files by vim:
  cat $SNAP_VIM
}

restore_vim () {
  echo -------------------------------------------------------------------------
  echo The last files opened by VIM are:
  cat $SNAP_VIM
}

snap_firefox () {
  #firefox auto restart the session when it is shutdown "accidentally"
  echo -------------------------------------------------------------------------
  echo Snapping firefox \(actually checking existence of firefox\)
  local check=`pgrep firefox`
  echo 0 > $SNAP_FIREFOX
  if [ "$check" != "" ]; then
    killall -9 firefox
    echo 1 > $SNAP_FIREFOX
  fi
}

restore_firefox () {
  echo -------------------------------------------------------------------------
  echo Restoring firefox ...
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
  #snap_firefox
}

restore () {
  restore_console
  restore_evince
  restore_vim
  #restore_firefox
}

cmdz=$1
echo Hello to my small program for $cmdz-ing current/last tasks ...
echo Let's retire our memory ...

if [ "${cmdz}" == "snap" ]; then
  echo Do snapping ...
  snap
else
  echo Do restoring ...
  restore
fi

