#!/bin/bash

doxygen qilfau.Doxyfile
cd ../docs/latex
make
cp refman.pdf ../qilfau-refman.pdf
cd ..
rm -fr latex