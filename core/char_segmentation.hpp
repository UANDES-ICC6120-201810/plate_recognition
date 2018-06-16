#ifndef CHAR_SEGMENTATION_H
#define CHAR_SEGMENTATION_H

#include <vector>

#include <opencv2/core/mat.hpp>


using namespace std;


class CharSegmentation {
public:
    vector< cv::Mat > findPlateCharImages( cv::Mat *plate_img );

private:
    vector< cv::Rect > findCharContours( cv::Mat *plate_img );

    vector< cv::Rect> getCharRects( cv::Mat *binary_img );

    cv::Rect getHorizontalCropPlateRect( cv::Mat *binary_img );
    bool isRowEmpty( cv::Mat *binary_img, int row_index );
    bool isValidPlateCropRect( cv::Rect plate_crop_rect );

    vector< cv::Rect > getVerticalCropCharRects( cv::Mat *plate_img, int original_y_offset );
    bool isColEmpty( cv::Mat *binary_img, int col_index );

    vector< cv::Rect > sortPlateCharsByX( vector< cv::Rect > plate_chars );

    vector< cv::Mat > getPlateCharImages( cv::Mat *plate_img, vector< cv::Rect > char_contours );
};

#endif
