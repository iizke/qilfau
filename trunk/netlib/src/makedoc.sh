#!/bin/bash

doxygen qilfau.Doxyfile
cd ../docs/latex
make
cp refman.pdf ../qilfau-refman.pdf
cd ..
rm -fr latex
tar cjf qilfau-refman.tar.bz2 qilfau-refman.pdf
rm -f qilfau-refman.pdf