#ifndef OCR_DETECTOR_H
#define OCR_DETECTOR_H

#include <string>
#include <opencv2/core/mat.hpp>

using namespace std;

class OcrDetector {
public:
    string plateCharsToString( vector< cv::Mat > char_images );
    char charImageToChar( cv::Mat char_image);
};

#endif
