#!/bin/bash


pushd /root/ALPR/ > /dev/null 2>&1
TRAINING_FILE_NAME="trained-svm.xml"
TOTAL_STEPS=3

echo
echo "Searching for training file '$TRAINING_FILE_NAME'"
if [ -f "$TRAINING_FILE_NAME" ]
then
    echo "File found..."
    echo "Skipping training: $TRAINING_FILE_NAME exists"
else
    echo "File not found..."
    echo "Step 1/$TOTAL_STEPS : Compiling SVM training"
    make -f scripts/makefile-train-svm

    if [ $? -eq 0 ]
    then
        echo "SVM compiled correctly!"
        echo

        echo "Step 3/$TOTAL_STEPS : Cleaning .DS_Store"
        rm ./data/.DS_Store
        find ./data/* -type f \( -name ".DS_Store" \) -delete
        echo

        echo "Step 2/$TOTAL_STEPS : Training SVM"
        ./TrainSVM
    else
        echo "SVM compile failed"
        exit 1
    fi
fi
popd > /dev/null 2>&1
