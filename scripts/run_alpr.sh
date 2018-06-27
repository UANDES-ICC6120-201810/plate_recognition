#!/bin/bash

pushd /root/ > /dev/null 2>&1
echo
echo "Step 1/1 : Starting ALPR"
time ./MainALPR.o
popd > /dev/null 2>&1