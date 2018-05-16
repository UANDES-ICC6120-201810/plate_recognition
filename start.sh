#!/bin/bash

TOTAL_STEPS="4"

echo "(1 of $TOTAL_STEPS) Compiling SVM training" \
&& g++ TrainSVM.cpp core/svm_char_detector.cpp $(pkg-config --libs opencv) -o TrainSVM \
&& echo "SVM compiled correctly!"\
&& echo \
&& echo "(2 of $TOTAL_STEPS) Training SVM" \
&& ./TrainSVM \
&& echo \
&& echo "(3 of $TOTAL_STEPS) Compiling ALPR" \
&& g++ MainALPR.cpp core/plate_segmentation.cpp core/char_segmentation.cpp core/ocr_detector.cpp core/svm_char_detector.cpp core/constants.hpp core/debugger.cpp $(pkg-config --libs opencv) -o MainALPR \
&& echo "ALPR compiled correctly!" \
&& echo \
&& echo "(4 of $TOTAL_STEPS) Starting ALPR" \
&& ./MainALPR