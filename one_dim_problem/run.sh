#!/usr/bin/env bash

echo "========== Building..."
cd "$(dirname $0)"
./build.sh
if [ $? -ne 0 ]; then
	exit 1
fi

echo "========== Running..."
cd bin
./one_dim