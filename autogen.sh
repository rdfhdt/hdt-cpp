#!/usr/bin/env bash

#Before running this file, make sure that gnulib.sh was executed
# or that m4 directory contains gnulib-*.m4 files 

aclocal -I m4 --install
autoreconf --install --force
