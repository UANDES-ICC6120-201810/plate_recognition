#!/bin/bash

pushd /root/ALPR/
./scripts/train_svm.sh && ./scripts/compile_alpr.sh && ./scripts/run_alpr.sh
popd
