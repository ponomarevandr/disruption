#!/usr/bin/env bash

cd "$(dirname $0)"
./build.sh
if [ $? -ne 0 ]; then
	exit 1
fi

echo "==================== Starting the program..."
cd bin
./tuning $@