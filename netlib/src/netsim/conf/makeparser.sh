#!/bin/bash
#rm lexer.c lexer.h parser.c parser.h channellexer.c channellexer.c channelparser.c channelparser.h

bison -y --defines=parser.h -o parser.c -k parser.y
#bison --d parser.y
flex --8bit --bison-bridge --header-file=lexer.h -o lexer.c lexer.l
#gcc -o parser parser.c lexer.c

bison -p nq -y --defines=channelparser.h -o channelparser.c -k channelparser.y
flex -P nq --8bit --bison-bridge --header-file=channellexer.h -o channellexer.c channellexer.l

bison -p mp -y --defines=markov_parser.h -o markov_parser.c -k markov_parser.y
flex -P mp --8bit --bison-bridge --header-file=markov_lexer.h -o markov_lexer.c markov_lexer.l
#gcc -o markov_parser markov_parser.c markov_lexer.c

