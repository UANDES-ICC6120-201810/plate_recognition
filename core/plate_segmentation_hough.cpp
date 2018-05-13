#include "plate_segmentation.hpp"

#include <opencv2/core/mat.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>

#include "Constants.hpp"

vector< cv::Mat > PlateSegmentation::findPlateImagesH(cv::Mat source_img) {
    cv::Mat source_edges = sourceImageToEdges(source_img);
    vector< vector< cv::Point > > plates_polygons = getContours(source_edges);
    vector< cv::Mat > plate_images;

    for( int index = 0; index < plates_polygons.size(); index++ ) {
        cv::Mat wrapped_plate = WrapPlateContour( source_img, plates_polygons[ index ] );
        wrapped_plate = sourceImageToBinary( wrapped_plate );

        plate_images.push_back( wrapped_plate );
    }

    return plate_images;
}

cv::Mat PlateSegmentation::sourceImageToEdges(cv::Mat source_image) {
    cv::Mat source_gray;
    cv::cvtColor(source_image, source_gray, cv::COLOR_BGR2GRAY);

    cv::blur(source_gray, source_gray, cv::Size(3, 3));

    cv::Mat source_edges;
    cv::Canny(source_gray, source_edges, 100, 300);

    return source_edges;
}

vector< vector< cv::Point > > PlateSegmentation::getContours( cv::Mat & source_edges ) {
    vector< vector< cv::Point > > contours;
    cv::findContours( source_edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE );

    /// Find the convex hull object for each contour

    vector< vector< cv::Point > > polygons;

    for ( int index = 0; index < contours.size(); index++ ) {
        cv::Mat contour( contours[ index ] );
        vector< cv::Point > hull;

        convexHull( contour, hull, false );

        vector< cv::Point > polygon;

        cv::approxPolyDP( hull, polygon, 20, true );

        if (polygon.size() == 4 ) {
            polygons.push_back( polygon );
        }
    }

    return polygons;
}

cv::Mat PlateSegmentation::WrapPlateContour( cv::Mat source_img, vector< cv::Point > plate_polygons ) {

    cv::Mat warpped_plate( PLATE_HEIGHT, PLATE_WIDTH, CV_8UC3 );
    vector< cv::Point> real_plate_polygons;
    real_plate_polygons = {cv::Point(PLATE_WIDTH, PLATE_HEIGHT), cv::Point(0, PLATE_HEIGHT), cv::Point(0, 0), cv::Point(PLATE_WIDTH, 0)};
    cv::Mat homography = cv::findHomography( plate_polygons, real_plate_polygons );
    cv::warpPerspective(source_img, warpped_plate, homography, cv::Size(PLATE_WIDTH, PLATE_HEIGHT));

    return warpped_plate;
}