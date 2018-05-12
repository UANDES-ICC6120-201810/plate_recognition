#ifndef ALPR_DEBUGGER_H
#define ALPR_DEBUGGER_H

#include <vector>
#include <opencv2/core/mat.hpp>

using namespace std;

void debugWriteRecognizedPlates(cv::Mat source_img, vector< cv::Rect > plates_rect);
void debugWriteRecognizedPlate(cv::Mat source_img, cv::Rect plate_rect);

void debugWriteRecognizedPlates(vector< cv::Mat > source_img);
void debugWriteRecognizedPlate(cv::Mat source_img);

void debugWriteRecognizedChars(cv::Mat source_img, vector< cv::Rect > char_images);
void debugWriteRecognizedChar(cv::Mat source_img, cv::Rect plate_char_rect);

void debugWriteRecognizedChars(vector< cv::Mat > source_img);
void debugWriteRecognizedChar(cv::Mat source_img);

void debugWriteImage(cv::Mat source_img, cv::Rect trim_rect, string file_name);

void debugWriteImage(cv::Mat source_img, string file_name);

#endif
