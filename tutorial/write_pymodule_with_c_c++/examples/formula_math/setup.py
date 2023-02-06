# -*- coding:utf-8 -*-

from distutils.core import setup, Extension

setup(name='formula_math', version='1.0',  \
		      ext_modules=[Extension('formula_math', ['src/formula_math.cpp'])])
