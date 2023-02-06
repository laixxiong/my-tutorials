#!/bin/bash

rm -fr build
rm -f *.so

# For py2.7
python setup.py build_ext --inplace
# For py3.11
#CC=gcc-6 CXX=g++-6 ~/bin/py311/bin/python3.11 setup.py build_ext --inplace
