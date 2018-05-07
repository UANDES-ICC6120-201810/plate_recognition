#ifndef ALPR_CONSTANTS_H
#define ALPR_CONSTANTS_H

#include <string>

using namespace std;

const string SVM_TRAINED_DATA_PATH = "svm.txt";

const int CLASSES_AMOUNT = 30;
const int CLASS_SAMPLES_AMOUNT = 10;
const int SVM_FEATURES_AMOUNT = 32;

const int PLATE_CHARS = 6;
const double PLATE_WIDTH = 36;
const double PLATE_HEIGHT = 13;

const double PLATE_RATIO_ERROR_RANGE = 0.2;

const double MIN_CHAR_WIDTH_PIXELS = 5;
const double MIN_CHAR_HEIGHT_PIXELS = 10;
const double MIN_PLATE_CHAR_HEIGHT_PERCENTAGE = 0.5;  // Min char height compared to the plate height in %

const double MIN_PLATE_WIDTH = PLATE_CHARS * MIN_CHAR_WIDTH_PIXELS;
const double MIN_PLATE_HEIGHT = MIN_CHAR_HEIGHT_PIXELS;

const double PLATE_RATIO = PLATE_WIDTH / PLATE_HEIGHT;
const double MAX_PLATE_RATIO = PLATE_RATIO * (1 + PLATE_RATIO_ERROR_RANGE);
const double MIN_PLATE_RATIO = PLATE_RATIO * (1 - PLATE_RATIO_ERROR_RANGE);

const double MIN_VALID_PIXEL_RATIO = 0.2;
const double MAX_VALID_PIXEL_RATIO = 0.7;

const int UPPER = 0;
const int LOWER = 1;
const int LEFT = 0;
const int RIGHT = 1;


#endif
