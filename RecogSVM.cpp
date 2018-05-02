#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/ml.hpp"
#include "dirent.h"
#include "feature.h"
#include "recogsvm.h"
#include <iostream>

using namespace cv;
using namespace std;
using namespace cv::ml;

const string SVM_TRAINED_DATA_PATH = "svm.txt";

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


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Missing File Argument" << endl;
        return 1;
    }

    Mat source_image = imread(argv[1]);

    string licence_text = SVMPredict(source_image);

    cout << licence_text << endl;

    return 0;
}

string SVMPredict(Mat source_image) {
    Mat original_binary_src_img = sourceImageToBinary(source_image);

    vector< vector<cv::Point> > potential_plates_contours = getPotentialPlatesContours(original_binary_src_img);

    if (potential_plates_contours.size() <= 0) return "";

    vector< vector<Mat> > detected_plates = getDetectedPlates(original_binary_src_img, potential_plates_contours);

    // Plate recognition
    string licence_text;
    for (size_t plate_index = 0; plate_index < detected_plates.size(); plate_index++) {
        string result = plateCharsToString(detected_plates.at(plate_index));
        licence_text += result;
    }
    return licence_text;
}

int plate_count = 0;
void debugRecognizedPlates(Mat source_img, Rect plate_rect) {

    Mat draw_image = source_img.clone();

    if (false) {
        int THICKNESS = 2;

        // Draw rect over image
        cv::rectangle(draw_image, plate_rect, Scalar(255, 0, 0), THICKNESS);
    } else {
        draw_image = source_img(plate_rect);
    }

    vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);

    string file_name = "./out/image" + to_string(plate_count) + ".jpg";

    cv::imwrite(file_name, draw_image, compression_params);
    plate_count++;
}

Mat sourceImageToBinary(Mat source_image) {
    Mat gray_source_image;
    cv::cvtColor(source_image, gray_source_image, cv::COLOR_BGR2GRAY);

    Mat binary_src_img;
    cv::adaptiveThreshold(gray_source_image, binary_src_img, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 55, 5);

    return binary_src_img;
}

vector< vector<cv::Point> > getPotentialPlatesContours(Mat original_binary_src_img) {
    Mat binary_src_img = original_binary_src_img.clone();
    Mat element = cv::getStructuringElement(MORPH_RECT, cv::Size(3, 3));
    cv::erode(binary_src_img, binary_src_img, element);
    cv::dilate(binary_src_img, binary_src_img, element);

    vector< vector<cv::Point> > potential_plates_contours;
    vector<Vec4i> hierarchy;
    cv::findContours(binary_src_img, potential_plates_contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    return potential_plates_contours;
}

vector< vector<Mat> > getDetectedPlates(Mat original_binary_src_img, vector< vector<cv::Point> > potential_plates_contours) {
    vector< vector<Mat> > detected_plates;

    for (size_t contour_index = 0; contour_index < potential_plates_contours.size(); contour_index++) {
        Rect plate_rect = cv::boundingRect(potential_plates_contours.at(contour_index));

        if (!validPlateDimensions(plate_rect)) continue;

        Mat plate_image = original_binary_src_img(plate_rect);

        vector< vector<cv::Point> > potential_plate_chars_contours = getPotentialPlatesCharsContours(plate_image);

        bool notEnoughPotentialCharsDetected = potential_plate_chars_contours.size() < PLATE_CHARS + 1;
        if (notEnoughPotentialCharsDetected) continue;

        vector<Rect> detected_plate_chars = getDetectedPlateChars(plate_rect, plate_image, potential_plate_chars_contours);

        bool notEnoughCharsDetected = detected_plate_chars.size() < PLATE_CHARS;
        if (notEnoughCharsDetected) continue;

        vector<Rect> sorted_plate_chars = sortPlateCharsByX(detected_plate_chars);

        vector<Mat> detected_plate_char_images = getCharImages(plate_image, sorted_plate_chars);

        detected_plates.push_back(detected_plate_char_images);
    }
    return detected_plates;
}

bool validPlateDimensions(Rect plate_rect) {
    double plate_ratio = (double) plate_rect.width / plate_rect.height;

    return MIN_PLATE_RATIO < plate_ratio
           && plate_ratio < MAX_PLATE_RATIO
           && plate_rect.width > MIN_PLATE_WIDTH
           && plate_rect.height > MIN_PLATE_HEIGHT;
}

vector< vector<cv::Point> > getPotentialPlatesCharsContours(Mat plate_image) {
    // Required by the function cv::findContours but unused
    //     Gets modified, so we make a copy to keep the original plate_image
    Mat unused_img = plate_image.clone();

    // Required by the function cv::findContours but unused
    vector<Vec4i> unused_hierarchy;

    vector< vector<cv::Point> > potential_plate_chars_contours;
    cv::findContours(unused_img, potential_plate_chars_contours, unused_hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    return potential_plate_chars_contours;
}

vector<Rect> getDetectedPlateChars(Rect plate_rect, Mat plate_image, vector<vector<cv::Point> > potential_plate_chars_contours) {
    vector<Rect> detected_plate_chars;
    int potential_plate_chars_amount = potential_plate_chars_contours.size();

    for (size_t char_index = 0; char_index < potential_plate_chars_amount; char_index++) {
        Rect plate_char_rect = cv::boundingRect(potential_plate_chars_contours.at(char_index));

        if (validPlateCharDimensions(plate_rect, plate_char_rect) ) {

            debugRecognizedPlates(plate_image, plate_char_rect);

            Mat plate_char_image = plate_image(plate_char_rect);

            if ( imageShouldBeChar(plate_char_image) ) {
                detected_plate_chars.push_back(plate_char_rect);
            }
        }
    }
    return detected_plate_chars;
}

bool imageShouldBeChar(Mat plate_char_image) {
    double black_pixels_ratio = (double) colorPixelsAmount(plate_char_image) / (plate_char_image.cols * plate_char_image.rows);
    return validPixelRatio(black_pixels_ratio);
}

// Checks whether a pixel_ratio represents a char or not
bool validPixelRatio(double pixel_ratio) {
    return MIN_VALID_PIXEL_RATIO < pixel_ratio && pixel_ratio < MAX_VALID_PIXEL_RATIO;
}

bool validPlateCharDimensions(Rect plate_rect, Rect sub_r) {

    double min_char_height = plate_rect.height * MIN_PLATE_CHAR_HEIGHT_PERCENTAGE;
    double max_char_width = plate_rect.width / (PLATE_CHARS - 1);

    return sub_r.height > min_char_height
           && MIN_CHAR_WIDTH_PIXELS < sub_r.width
           && sub_r.width < max_char_width;
}

vector<Rect> sortPlateCharsByX(vector<Rect> detected_plate_chars) {
    int detected_plate_chars_amount = detected_plate_chars.size();

    for (int i = 0; i < detected_plate_chars_amount - 1; i++) {
        for (int j = i + 1; j < detected_plate_chars_amount; j++) {

            if (detected_plate_chars.at(j).x < detected_plate_chars.at(i).x) {
                Rect temp = detected_plate_chars.at(j);
                detected_plate_chars.at(j) = detected_plate_chars.at(i);
                detected_plate_chars.at(i) = temp;
            }
        }
    }

    return detected_plate_chars;
}

vector<Mat> getCharImages(Mat plate_image, vector<Rect> detected_plate_chars) {
    vector<Mat> detected_plate_char_images;
    for (int char_index = 0; char_index < detected_plate_chars.size(); char_index++) {
        Mat plate_char_image = plate_image(detected_plate_chars.at(char_index));
        detected_plate_char_images.push_back(plate_char_image);
    }
    return detected_plate_char_images;
}

string plateCharsToString(vector<Mat> detected_plate) {
    string licence_text;
    for (size_t char_index = 0; char_index < detected_plate.size(); ++char_index) {
        char plate_char = detectCharFromImage(detected_plate.at(char_index));
        licence_text.push_back(plate_char);
    }
    return licence_text;
}

char detectCharFromImage(Mat char_image) {
    //Load SVM training file OpenCV 3.1
    Ptr <SVM> svmNew = SVM::create();
    svmNew = SVM::load(SVM_TRAINED_DATA_PATH);


    vector<float> image_features = calculateImageFeatures(char_image);
    // Open CV3.1
    Mat m = Mat(1, number_of_feature, CV_32FC1);
    for (size_t i = 0; i < image_features.size(); ++i) {
        float temp = image_features[i];
        m.at<float>(0, i) = temp;
    }

    int ri = int(svmNew->predict(m)); // Open CV 3.1

    char c = '*';
    if (ri >= 0 && ri <= 9)
        c = (char) (ri + 48); //ma ascii 0 = 48
    if (ri >= 10 && ri < 18)
        c = (char) (ri + 55); //ma accii A = 5, --> tu A-H
    if (ri >= 18 && ri < 22)
        c = (char) (ri + 55 + 2); //K-N, bo I,J
    if (ri == 22) c = 'P';
    if (ri == 23) c = 'S';
    if (ri >= 24 && ri < 27)
        c = (char) (ri + 60); //T-V,
    if (ri >= 27 && ri < 30)
        c = (char) (ri + 61); //X-Z

    return c;

}
