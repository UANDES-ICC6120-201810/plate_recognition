#include "CharacterFinder.hpp"

#include <vector>
#include <iostream>  // Remove later

#include <opencv2/core/mat.hpp>

#include "Constants.hpp"
#include "debugger.hpp"  // Remove later
#include "SVMCharDetector.hpp"

vector< string > CharacterFinder::findPlatesText(vector< cv::Mat > plate_images) {
    vector< string > plates_text;

    for (size_t index = 0; index < plate_images.size(); index++) {
        string plate_text = findPlateText(plate_images.at(index));

        if (plate_text.length() == PLATE_CHARS)
            plates_text.push_back(plate_text);
    }
    return plates_text;
}


string CharacterFinder::findPlateText(cv::Mat plate_img) {
    vector< cv::Rect > potential_char_contours = findPotentialCharContours(plate_img);

    cv::Rect plate_rect = cv::Rect(0, 0, plate_img.size().width, plate_img.size().height);

    vector< cv::Rect > valid_char_contours = removeInvalidCharContours(plate_img, potential_char_contours);

    if (valid_char_contours.size() < PLATE_CHARS) {
        cout << "Fall down to horizontal and vertical smearing" << endl;
        valid_char_contours = getCharRects(plate_img);
    };

    if (valid_char_contours.size() < PLATE_CHARS) return "";

    vector< cv::Rect > sorted_chars = sortPlateCharsByX(valid_char_contours);
    if (sorted_chars.size() < PLATE_CHARS) return "";

    debugWriteRecognizedChars(plate_img, sorted_chars);

    vector< cv::Mat > char_images = getPlateCharImages(plate_img, sorted_chars);

    return plateCharsToString(char_images);
}


vector< cv::Rect > CharacterFinder::findPotentialCharContours(cv::Mat plate_img) {
    cv::Mat unused_img = plate_img.clone();
    vector< cv::Vec4i > unused_hierarchy;

    vector< vector< cv::Point > > char_contours;
    cv::findContours(unused_img, char_contours, unused_hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    return transformContoursToRects(char_contours);
}


vector< cv::Rect > CharacterFinder::transformContoursToRects(vector< vector< cv::Point > > char_contours) {
    vector< cv::Rect > plate_rects;

    for (size_t index = 0; index < char_contours.size(); index++) {
        cv::Rect plate_rect = cv::boundingRect(char_contours.at(index));
        plate_rects.push_back(plate_rect);
    }

    return plate_rects;
}


vector< cv::Rect > CharacterFinder::removeInvalidCharContours(cv::Mat plate_img, vector< cv::Rect > char_contours) {
    vector< cv::Rect > valid_contours;
    size_t total_contours = char_contours.size();

    for (size_t index = 0; index < char_contours.size(); index++) {
        cv::Rect char_rect = char_contours.at(index);

        if (!validPlateCharDimensions(plate_img, char_rect)) continue;

        cv::Mat char_img = plate_img(char_rect);
        if (!validCharImage(char_img)) continue;

        valid_contours.push_back(char_rect);
    }

    return valid_contours;
}


bool CharacterFinder::validPlateCharDimensions(cv::Mat plate_img, cv::Rect char_rect) {
    double min_char_height = plate_img.size().height * MIN_PLATE_CHAR_HEIGHT_PERCENTAGE;
    double max_char_width = plate_img.size().width / (PLATE_CHARS - 1);

    return char_rect.height > min_char_height
           && MIN_CHAR_WIDTH_PIXELS < char_rect.width
           && char_rect.width < max_char_width;
}


bool CharacterFinder::validCharImage(cv::Mat char_img) {
    bool check_black_pixels = true;
    double black_pixels_amount = colorPixelsAmount(char_img, check_black_pixels);
    int total_pixels = char_img.cols * char_img.rows;

    double pixel_ratio = black_pixels_amount / total_pixels;

    return validPixelRatio(pixel_ratio);
}


double CharacterFinder::colorPixelsAmount(cv::Mat img, bool black_pixel) {
    double black = 0;
    double white = 0;

    for (int row = 0; row < img.rows; ++row)
        for (int col = 0; col < img.cols; ++col) {
            if (img.at<uchar>(row, col) == 0)
                black++;
            else
                white++;
        }
    if (black_pixel)
        return black;
    else
        return white;
}

// Checks whether a pixel_ratio represents a char or not
bool CharacterFinder::validPixelRatio(double pixel_ratio) {
    return MIN_VALID_PIXEL_RATIO < pixel_ratio && pixel_ratio < MAX_VALID_PIXEL_RATIO;
}

vector< cv::Rect> CharacterFinder::getCharRects(cv::Mat binary_img) {
    const int PLATE_CHARS_WITH_MARGIN = PLATE_CHARS + 4;
    vector< cv::Rect > detected_chars;

    cv::Rect horizontalCrop = getHorizontalCropPlateRect(binary_img);

    if (!isValidCropRect(horizontalCrop)) return detected_chars;
    cv::Mat cropped_img = binary_img(horizontalCrop);

    int original_y_offset = horizontalCrop.y;

    int left_px = -1;
    int right_px = -1;
    bool previous_was_empty = false;
    int height = cropped_img.size().height;


    for (int i = 0; i < cropped_img.cols; i++) {
        double white_pixels = cv::countNonZero(cropped_img(cv::Rect(i, 0, 1, cropped_img.rows)));
        double white_pixel_percentage = white_pixels / height;

        bool is_empty = white_pixel_percentage > 0.95;

        bool detected_left_px = left_px != -1;
        bool detected_right_px = right_px != -1;

        bool is_left_px = previous_was_empty && is_empty && !detected_right_px;
        bool is_right_px = !previous_was_empty && is_empty && detected_left_px;

        if (is_left_px) left_px = i + 1;
        if (is_right_px) {
            right_px = i - 1;
            cv::Rect char_rect = getCharRect(left_px, right_px, height, original_y_offset);
            detected_chars.push_back(char_rect);

            left_px = -1;
            right_px = -1;
        }

        previous_was_empty = is_empty;
    }

    return detected_chars;
}

bool CharacterFinder::isValidCropRect(cv::Rect horizontalCrop) {
    return 0 < horizontalCrop.x
           && 0 < horizontalCrop.y
           && horizontalCrop.x < horizontalCrop.width
           && horizontalCrop.y < horizontalCrop.height;
}

cv::Rect CharacterFinder::getCharRect(int left_margin, int right_margin, int image_height, int original_y_offset) {
    int char_width = right_margin - left_margin;
    return cv::Rect(left_margin, original_y_offset, char_width, image_height);
}

cv::Rect CharacterFinder::getHorizontalCropPlateRect(cv::Mat binary_img) {

    int width = binary_img.size().width;

    int upper_px = -1;
    int lower_px = -1;

    bool previous_was_empty = false;

    for (int i = 0; i < binary_img.rows - 1; i++) {

        double white_pixels = cv::countNonZero(binary_img(cv::Rect(0, i, binary_img.cols, 1)));
        double white_pixel_percentage = white_pixels / width;

        bool is_empty = white_pixel_percentage > 0.8;
        bool detected_lower_px = lower_px != -1;
        bool detected_upper_px = upper_px != -1;

        bool is_upper_px = previous_was_empty && is_empty && !detected_lower_px;
        bool is_lower_px = !previous_was_empty && is_empty && detected_upper_px;

        if (is_upper_px) upper_px = i + 1;
        if (is_lower_px) lower_px = i;

        previous_was_empty = is_empty;
    }

    cv::Rect horizontalCrop = cv::Rect(0, upper_px, width, lower_px - upper_px);

    return horizontalCrop;
}


vector<cv::Rect> CharacterFinder::sortPlateCharsByX(vector< cv::Rect > plate_chars) {
    int detected_plate_chars_amount = plate_chars.size();

    for (int i = 0; i < detected_plate_chars_amount - 1; i++) {
        for (int j = i + 1; j < detected_plate_chars_amount; j++) {

            if (plate_chars.at(j).x < plate_chars.at(i).x) {
                cv::Rect temp = plate_chars.at(j);
                plate_chars.at(j) = plate_chars.at(i);
                plate_chars.at(i) = temp;
            }
        }
    }

    return plate_chars;
}


vector< cv::Mat > CharacterFinder::getPlateCharImages(cv::Mat plate_img, vector< cv::Rect > char_contours) {
    vector< cv::Mat > char_images;

    for (size_t index = 0; index < char_contours.size(); index++) {
        cv::Mat char_image = getCharImage(plate_img, char_contours.at(index));

        char_images.push_back(char_image);
    }

    return char_images;
}


cv::Mat CharacterFinder::getCharImage(cv::Mat plate_img, cv::Rect char_contour) {
    return plate_img(char_contour);
}


string CharacterFinder::plateCharsToString(vector< cv::Mat > char_images) {
    string licence_text = "";

    for (size_t index = 0; index < char_images.size(); index++) {
        char plate_char = detectCharFromImage(char_images.at(index));
        licence_text += plate_char;
    }

    return licence_text;
}