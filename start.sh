#!/bin/bash

TOTAL_STEPS="4"

echo "Step 1/$TOTAL_STEPS : Compiling SVM training" \
&& g++ TrainSVM.cpp core/svm_char_detector.cpp $(pkg-config --libs opencv) -o TrainSVM \
&& echo "SVM compiled correctly!"\
&& echo \
&& echo "Step 2/$TOTAL_STEPS : Training SVM" \
&& ./TrainSVM \
&& echo \
&& echo "Step 3/$TOTAL_STEPS : Compiling ALPR" \
&& g++ MainALPR.cpp core/plate_segmentation.cpp core/char_segmentation.cpp core/ocr_detector.cpp core/svm_char_detector.cpp core/constants.hpp core/debugger.cpp $(pkg-config --libs opencv) -o MainALPR \
&& echo "ALPR compiled correctly!" \
&& echo \
&& echo "Step 4/$TOTAL_STEPS : Starting ALPR" \
&& ./MainALPR
