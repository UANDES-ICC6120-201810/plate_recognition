#include "char_segmentation.hpp"

#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "constants.hpp"
#include "debugger.hpp"

using namespace std;


vector< cv::Mat > CharSegmentation::findPlateCharImages( cv::Mat plate_img ) {
    vector< cv::Rect > char_contours = findCharContours( plate_img );
    vector< cv::Mat > char_images = getPlateCharImages( plate_img, char_contours );

    debugWriteRecognizedPlate( plate_img );
    debugWriteRecognizedChars( char_images );

    return char_images;
}


vector< cv::Rect > CharSegmentation::findCharContours( cv::Mat plate_img ) {
    vector< cv::Rect > valid_contours = getCharRects( plate_img );

    bool detected_enough_chars = valid_contours.size() >= PLATE_CHARS;
    if ( !detected_enough_chars ) return {};

    vector< cv::Rect > sorted_chars = sortPlateCharsByX( valid_contours );

    return sorted_chars;
}


vector< cv::Rect> CharSegmentation::getCharRects( cv::Mat binary_img ) {
    cv::Rect horizontalCrop = getHorizontalCropPlateRect(binary_img);
    bool isNotValidCrop = !isValidPlateCropRect( horizontalCrop );

    if ( isNotValidCrop ) return {};

    cv::Mat cropped_img = binary_img( horizontalCrop );
    int original_y_offset = horizontalCrop.y;

    vector< cv::Rect > detected_chars = getVerticalCropCharRects( cropped_img, original_y_offset );

    return detected_chars;
}


cv::Rect CharSegmentation::getHorizontalCropPlateRect( cv::Mat binary_img ) {
    const int img_width = binary_img.cols;
    const int img_height = binary_img.rows;

    int upper_px = 0;
    int lower_px = -1;

    bool previous_was_empty = false;

    cv::Rect horizontalCrop;

    for ( int row_index = 0; row_index < img_height; row_index++ ) {
        bool is_row_empty = isRowEmpty( binary_img, row_index );

        bool detected_lower_px = lower_px != -1;

        bool is_upper_px = previous_was_empty && is_row_empty && !detected_lower_px;
        bool is_lower_px = !previous_was_empty && is_row_empty;

        if ( is_upper_px ) upper_px = row_index + 1;
        if ( is_lower_px ) {
            lower_px = row_index - 1;

            int char_region_height = lower_px - upper_px;

            if ( char_region_height > MIN_CHAR_HEIGHT_PIXELS )
                horizontalCrop = cv::Rect( 0, upper_px, img_width, char_region_height );

            upper_px = row_index;
            lower_px = -1;
        }

        previous_was_empty = is_row_empty;
    }

    return horizontalCrop;
}


bool CharSegmentation::isRowEmpty( cv::Mat binary_img, int row_index ) {
    cv::Mat row_img = binary_img( cv::Rect( 0, row_index, binary_img.cols, 1 ) );

    double row_white_pixels = cv::countNonZero( row_img );

    double white_pixel_percentage = row_white_pixels / binary_img.cols;

    return white_pixel_percentage > EMPTY_ROW_RATIO;
}


bool CharSegmentation::isValidPlateCropRect( cv::Rect plate_crop_rect ) {
    return 0 <= plate_crop_rect.x
           && 0 <= plate_crop_rect.y
           && plate_crop_rect.x < plate_crop_rect.width
           && plate_crop_rect.y < plate_crop_rect.height;
}


vector< cv::Rect > CharSegmentation::getVerticalCropCharRects( cv::Mat plate_img, int original_y_offset ) {
    const int char_height = plate_img.size().height;

    int left_x_coord = 0;
    int right_x_coord = -1;

    bool previous_was_empty = false;

    vector< cv::Rect > detected_chars;

    for ( int col_index = 0; col_index < plate_img.cols; col_index++ ) {
        bool is_col_empty = isColEmpty( plate_img, col_index );

        bool detected_right_px = right_x_coord != -1;

        bool is_left_x_coord = previous_was_empty && is_col_empty && !detected_right_px;
        bool is_right_x_coord = !previous_was_empty && is_col_empty;

        if ( is_left_x_coord ) left_x_coord = col_index + 1;
        if ( is_right_x_coord ) {
            right_x_coord = col_index - 1;

            int char_width = right_x_coord - left_x_coord;

            if ( char_width > MIN_CHAR_WIDTH_PIXELS ) {
                cv::Rect char_rect = cv::Rect( left_x_coord, original_y_offset, char_width, char_height );
                detected_chars.push_back( char_rect );
            }

            left_x_coord = col_index;
            right_x_coord = -1;
        }

        previous_was_empty = is_col_empty;
    }

    return detected_chars;
}


bool CharSegmentation::isColEmpty( cv::Mat binary_img, int col_index ) {
    cv::Mat row_img = binary_img( cv::Rect( col_index, 0, 1, binary_img.rows ) );

    double col_white_pixels = cv::countNonZero( row_img );

    double white_pixel_percentage = col_white_pixels / binary_img.rows;

    return white_pixel_percentage > EMPTY_COL_RATIO;
}


bool sortRectsByX( cv::Rect first, cv::Rect second ) {
    return first.x < second.x;
}


vector<cv::Rect> CharSegmentation::sortPlateCharsByX( vector< cv::Rect > plate_chars ) {
    int chars_amount = plate_chars.size();

    sort( plate_chars.begin(), plate_chars.end(), sortRectsByX);

    return plate_chars;
}


vector< cv::Mat > CharSegmentation::getPlateCharImages(cv::Mat plate_img, vector< cv::Rect > char_contours) {
    vector< cv::Mat > char_images;

    for ( size_t index = 0; index < char_contours.size(); index++ ) {
        cv::Rect char_contour = char_contours.at( index );
        cv::Mat char_image = plate_img( char_contour );

        char_images.push_back( char_image );
    }

    return char_images;
}