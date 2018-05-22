#!/bin/bash

pushd /root/ALPR/
./train_svm.sh && ./compile_alpr.sh && ./run_alpr.sh
popd
