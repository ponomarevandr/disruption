#!/usr/bin/env bash

cd "$(dirname $0)"
mkdir -p bin
g++ -O2 -o bin/one_dim one_dim.cpp