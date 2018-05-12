#ifndef CHAR_SEGMENTATION_H
#define CHAR_SEGMENTATION_H

#include <vector>

#include <opencv2/core/mat.hpp>


using namespace std;


class CharSegmentation {
public:
    vector< cv::Mat > findPlateCharImages(cv::Mat plate_img);

private:
    vector< cv::Rect > findCharContours(cv::Mat plate_img);
    vector< cv::Rect > findPotentialCharContours(cv::Mat plate_img);
    vector< cv::Rect > transformContoursToRects(vector< vector< cv::Point > > char_contours);
    vector< cv::Rect > removeInvalidCharContours(cv::Mat plate_image, vector< cv::Rect > potential_char_contours);
    bool validPlateCharDimensions(cv::Mat plate_img, cv::Rect char_rect);
    bool validCharImage(cv::Mat char_img);
    double colorPixelsAmount(cv::Mat img, bool black_pixel);
    bool validPixelRatio(double pixel_ratio);
    vector< cv::Rect> getCharRects(cv::Mat binary_img);
    bool isValidCropRect(cv::Rect horizontalCrop);
    cv::Rect getCharRect(int left_margin, int right_margin, int image_height, int original_y_offset);
    cv::Rect getHorizontalCropPlateRect(cv::Mat binary_img);
    vector< cv::Rect > sortPlateCharsByX(vector< cv::Rect > plate_chars);
    vector< cv::Mat > getPlateCharImages(cv::Mat plate_img, vector< cv::Rect > char_contours);
    cv::Mat getCharImage(cv::Mat plate_img, cv::Rect char_contour);
};

#endif
