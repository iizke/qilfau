#!/bin/bash
rm lexer.c lexer.h parser.c parser.h

bison -y --defines=parser.h -o parser.c -k parser.y
#bison --d parser.y
flex --8bit --bison-bridge --header-file=lexer.h -o lexer.c lexer.l
#gcc -o parser parser.c lexer.c
