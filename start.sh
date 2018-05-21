#!/bin/bash

TOTAL_STEPS="4"

TRAINING_FILE_NAME="trained-svm.xml"

while test $# -gt 0
do
    case "$1" in
        --force) TRAINING_FILE_NAME=""
        ;;
        --file) TRAINING_FILE_NAME="$2"
        ;;
    esac
    shift
done

echo
echo "Pulling from git repo"
echo

git pull

echo
echo "Searching for training file '$TRAINING_FILE_NAME'"
if [ -f "$TRAINING_FILE_NAME" ]
then
    echo "File found..."
    echo "Skipping training: $TRAINING_FILE_NAME exists"
else
    echo "File not found..."
    echo "Step 1/$TOTAL_STEPS : Compiling SVM training" \
    && g++ TrainSVM.cpp core/svm_char_detector.cpp $(pkg-config --libs opencv) -o TrainSVM \
    && echo "SVM compiled correctly!"\
    && echo \
    && echo "Cleaning .DS_Store" \
    && rm ./data/.DS_Store \
    && find ./data/* -type f \( -name ".DS_Store" \) -delete \
    && echo "Step 2/$TOTAL_STEPS : Training SVM" \
    && ./TrainSVM
fi
echo \
&& echo "Step 3/$TOTAL_STEPS : Compiling ALPR" \
&& g++ MainALPR.cpp core/async_video_read.cpp -lpthread core/plate_segmentation.cpp core/char_segmentation.cpp core/ocr_detector.cpp core/svm_char_detector.cpp core/constants.hpp core/debugger.cpp $(pkg-config --libs opencv) -o MainALPR \
&& echo "ALPR compiled correctly!" \
&& echo \
&& echo "Step 4/$TOTAL_STEPS : Starting ALPR" \
&& time ./MainALPR
