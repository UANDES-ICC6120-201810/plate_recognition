#!/bin/bash

pushd /root/ALPR/
echo
echo "Step 1/1 : Compiling ALPR"
g++ MainALPR.cpp core/async_video_read.cpp -lpthread core/plate_segmentation.cpp core/char_segmentation.cpp core/ocr_detector.cpp core/svm_char_detector.cpp core/constants.hpp core/debugger.cpp $(pkg-config --libs opencv) -o MainALPR \

if [ $? -eq 0 ]
then
    echo "ALPR compiled correctly!"
else
    echo "ALPR compile failed"
fi
popd