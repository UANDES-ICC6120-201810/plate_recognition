#!/bin/bash

pushd /root/ > /dev/null 2>&1
./scripts/train_svm.sh && ./scripts/compile_alpr.sh && ./scripts/run_alpr.sh
popd > /dev/null 2>&1
