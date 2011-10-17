#!/bin/bash
#rm lexer.c lexer.h parser.c parser.h channellexer.c channellexer.c channelparser.c channelparser.h

CUR_DIR=`pwd`
_NAME=$BASH_SOURCE
_FILEDIR=`dirname $(which  $_NAME)`
cd ${_FILEDIR}

bison -p sched -y --defines=parser.h -o parser.c -k parser.y
#bison --d parser.y
flex -P sched --8bit --bison-bridge --header-file=lexer.h -o lexer.c lexer.l
#gcc -o parser parser.c lexer.c


