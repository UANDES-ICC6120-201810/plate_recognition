#!/bin/bash

pushd /root/ > /dev/null 2>&1
echo
echo "Step 1/1 : Compiling ALPR"
make -f scripts/makefile-alpr

if [ $? -eq 0 ]
then
    echo "ALPR compiled correctly!"
else
    echo "ALPR compile failed"
    popd > /dev/null 2>&1
    exit 1
fi
popd > /dev/null 2>&1