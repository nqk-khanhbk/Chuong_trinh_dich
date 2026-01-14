#!/bin/bash
# Script để xem bytecode instructions được generate

if [ -z "$1" ]; then
    echo "Usage: ./test_dump.sh <example_number>"
    echo "Example: ./test_dump.sh 2"
    exit 1
fi

NUM=$1
echo "=== Compiling example${NUM}.kpl with code dump ==="
echo ""
./kplc tests/example${NUM}.kpl output${NUM} -dump
echo ""
echo "=== Source code: ==="
cat tests/example${NUM}.kpl
