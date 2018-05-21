#include "ocr_detector.hpp"

#include <string>
#include <regex>
#include <opencv2/core/mat.hpp>

#include "constants.hpp"
#include "svm_char_detector.hpp"

using namespace std;


OcrDetector::OcrDetector( string trained_data_path) {
    svm_detector = new SvmCharDetector(trained_data_path);
}


string OcrDetector::plateCharsToString( vector< cv::Mat > char_images ) {
    string licence_text = "";

    for (size_t index = 0; index < char_images.size(); index++) {
        char plate_char = charImageToChar(char_images.at(index));
        licence_text += plate_char;

    }

    if ( matchesRegex( licence_text ) )
        return licence_text;

    return {};
}


char OcrDetector::charImageToChar( cv::Mat char_image ) {
    return svm_detector -> detectCharFromImage( char_image );
}

bool OcrDetector::matchesRegex( string license_text ) {
    regex plate_regex(PLATE_REGEX);
    return regex_match(license_text, plate_regex);
}