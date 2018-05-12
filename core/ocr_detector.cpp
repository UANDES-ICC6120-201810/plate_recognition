#include "ocr_detector.hpp"

#include <string>
#include <opencv2/core/mat.hpp>

#include "SVMCharDetector.hpp"


string OcrDetector::plateCharsToString( vector< cv::Mat > char_images ) {
    string licence_text = "";

    for (size_t index = 0; index < char_images.size(); index++) {
        char plate_char = charImageToChar(char_images.at(index));
        licence_text += plate_char;

    }

    return licence_text;
}

char OcrDetector::charImageToChar( cv::Mat char_image ) {
    return detectCharFromImage(char_image);
}