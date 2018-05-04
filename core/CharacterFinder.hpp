#ifndef ALPR_CHARACTERFINDER_H
#define ALPR_CHARACTERFINDER_H

#include <vector>

#include <opencv2/core/mat.hpp>


using namespace std;


class CharacterFinder {
public:
    vector< string > findPlatesText(vector< cv::Mat > plate_images);
    string findPlateText(cv::Mat plate_img);

private:
    vector< cv::Rect > findPotentialCharContours(cv::Mat plate_img);
    vector< cv::Rect > transformContoursToRects(vector< vector< cv::Point > > char_contours);
    vector< cv::Rect > removeInvalidCharContours(cv::Mat plate_image, vector< cv::Rect > potential_char_contours);
    bool validPlateCharDimensions(cv::Mat plate_img, cv::Rect char_rect);
    bool validCharImage(cv::Mat char_img);
    double colorPixelsAmount(cv::Mat img, bool black_pixel);
    bool validPixelRatio(double pixel_ratio);
    vector< cv::Rect > sortPlateCharsByX(vector< cv::Rect > plate_chars);
    vector< cv::Mat > getPlateCharImages(cv::Mat plate_img, vector< cv::Rect > char_contours);
    cv::Mat getCharImage(cv::Mat plate_img, cv::Rect char_contour);
    string plateCharsToString(vector< cv::Mat > char_images);
};

#endif
