#ifndef ALPR_CONSTANTS_H
#define ALPR_CONSTANTS_H

#include <string>

using namespace std;

const string SVM_TRAINED_DATA_PATH = "trained-svm.xml";
const string SVM_TRAINING_DATA_PATH = "./data/";

const int CLASSES_AMOUNT = 30;
const int CLASS_SAMPLES_AMOUNT = 10;
const int SVM_FEATURES_AMOUNT = 32;

const int PLATE_CHARS = 6;
const double PLATE_WIDTH = 360;
const double PLATE_HEIGHT = 130;
const string EMPTY_PLATE = "";

const double PLATE_RATIO_ERROR_RANGE = 0.2;

const double MIN_CHAR_WIDTH_PIXELS = 22;
const double MIN_CHAR_HEIGHT_PIXELS = PLATE_HEIGHT / 2;
const double MIN_PLATE_CHAR_HEIGHT_PERCENTAGE = 0.5;  // Min char height compared to the plate height in %

const double EMPTY_ROW_RATIO = 0.8;
const double EMPTY_COL_RATIO = 0.9;

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

const int FEATURE_WIDTH = 40;
const int FEATURE_HEIGHT = 40;

const int CELL_WIDTH = FEATURE_WIDTH / 4;
const int CELL_HEIGHT = FEATURE_HEIGHT / 4;


#endif
