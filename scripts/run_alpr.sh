#!/bin/bash

pushd /root/ALPR/ > /dev/null 2>&1
echo
echo "Step 1/1 : Starting ALPR"
time ./MainALPR
popd > /dev/null 2>&1