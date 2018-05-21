#ifndef OCR_DETECTOR_H
#define OCR_DETECTOR_H

#include <string>
#include <opencv2/core/mat.hpp>

#include "svm_char_detector.hpp"

using namespace std;

class OcrDetector {
public:
    OcrDetector( string trained_data_path);
    string plateCharsToString( vector< cv::Mat > char_images );

private:
    SvmCharDetector *svm_detector;
    char charImageToChar( cv::Mat char_image);
    bool matchesRegex( string license_text );
};

#endif
