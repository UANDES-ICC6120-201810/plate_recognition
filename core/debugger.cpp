#include "debugger.h"

#include <opencv2/core/mat.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;

int plate_file_name_counter = 0;

void debugWriteRecognizedPlates(cv::Mat source_img, vector< cv::Rect > plates_rect) {
    for (size_t index = 0; index < plates_rect.size(); index++) {
        debugWriteRecognizedPlate(source_img, plates_rect.at(index));
    }
}

void debugWriteRecognizedPlate(cv::Mat source_img, cv::Rect plate_rect) {
    string file_name = "./out/plate_" + to_string(plate_file_name_counter++) + ".jpg";

    debugWriteImage(source_img, plate_rect, file_name);
}

int char_file_name_counter = 0;

void debugWriteRecognizedChars(cv::Mat source_img, vector< cv::Rect > char_images) {
    for (size_t index = 0; index < char_images.size(); index++) {
        debugWriteRecognizedChar(source_img, char_images.at(index));
    }
}

void debugWriteRecognizedChar(cv::Mat source_img, cv::Rect plate_char_rect) {
    string file_name = "./out/plate_char_" + to_string(char_file_name_counter++) + ".jpg";

    debugWriteImage(source_img, plate_char_rect, file_name);
}

void debugWriteImage(cv::Mat source_img, cv::Rect trim_rect, string file_name) {
    cv::Mat trimmed_img = source_img(trim_rect);

    vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);

    cv::imwrite(file_name, trimmed_img, compression_params);
}