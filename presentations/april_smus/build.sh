#!/usr/bin/env bash

cd "$(dirname $0)"
mkdir -p build
cd build

pdflatex ../main.tex
biber main.bcf
pdflatex ../main.tex
