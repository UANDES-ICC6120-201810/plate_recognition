#include "PlateFinder.hpp"

#include <vector>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/mat.hpp>

#include "Constants.hpp"


using namespace std;


vector< cv::Mat > PlateFinder::findPlateImages(cv::Mat source_img) {
    cv::Mat binary_img = sourceImageToBinary(source_img);

    vector< cv::Rect > potential_plate_contours = findPotentialPlateContours(binary_img);
    vector< cv::Rect > valid_plate_contours = removeInvalidPlateContours(potential_plate_contours);

    return getPlateImages(binary_img, valid_plate_contours);
}


cv::Mat PlateFinder::sourceImageToBinary(cv::Mat source_image) {
    cv::Mat gray_source_image;
    cv::cvtColor(source_image, gray_source_image, cv::COLOR_BGR2GRAY);

    cv::Mat binary_img;
    cv::adaptiveThreshold(gray_source_image, binary_img, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 55, 5);

    return binary_img;
}



vector< cv::Rect > PlateFinder::findPotentialPlateContours(cv::Mat source_img) {
    cv::Mat binary_src_img = source_img.clone();

    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::erode(binary_src_img, binary_src_img, element);
    cv::dilate(binary_src_img, binary_src_img, element);

    vector< vector< cv::Point > > plate_contours;
    vector< cv::Vec4i > hierarchy;
    cv::findContours(binary_src_img, plate_contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    return transformContoursToRects(plate_contours);
}


vector< cv::Rect > PlateFinder::transformContoursToRects(vector< vector< cv::Point > > plate_contours) {
    vector< cv::Rect > plate_rects;

    for (size_t index = 0; index < plate_contours.size(); index++) {
        cv::Rect plate_rect = cv::boundingRect(plate_contours.at(index));
        plate_rects.push_back(plate_rect);
    }

    return plate_rects;
}


vector< cv::Rect > PlateFinder::removeInvalidPlateContours(vector< cv::Rect > plate_contours) {
    vector< cv::Rect > valid_contours;
    size_t total_contours = plate_contours.size();

    for (size_t index = 0; index < plate_contours.size(); index++) {
        cv::Rect plate_rect = plate_contours.at(index);

        if (!validPlateDimensions(plate_rect)) continue;

        valid_contours.push_back(plate_rect);
    }

    return valid_contours;
}


bool PlateFinder::validPlateDimensions(cv::Rect plate_rect) {
    double plate_ratio = (double) plate_rect.width / plate_rect.height;

    return MIN_PLATE_RATIO < plate_ratio
           && plate_ratio < MAX_PLATE_RATIO
           && plate_rect.width > MIN_PLATE_WIDTH
           && plate_rect.height > MIN_PLATE_HEIGHT;
}


vector< cv::Mat > PlateFinder::getPlateImages(cv::Mat source_img, vector< cv::Rect > plate_contours) {
    vector< cv::Mat > plate_images;

    for (size_t index = 0; index < plate_contours.size(); index++) {
        cv::Mat plate_image = getPlateImage(source_img, plate_contours.at(index));

        plate_images.push_back(plate_image);
    }

    return plate_images;
}


cv::Mat PlateFinder::getPlateImage(cv::Mat original_img, cv::Rect plate_contour) {
    return original_img(plate_contour);
}
