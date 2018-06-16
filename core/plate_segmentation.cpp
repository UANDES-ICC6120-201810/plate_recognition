#include "plate_segmentation.hpp"

#include <opencv2/core/mat.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <math.h>

#include "constants.hpp"

vector< cv::Mat > PlateSegmentation::findPlateImages( cv::Mat *source_img ) {
    cv::Mat source_edges = sourceImageToEdges(source_img);

    vector< vector< cv::Point > > plates_polygons = getPolygons( source_edges );
    vector< cv::Mat > plate_images;

    for( size_t index = 0; index < plates_polygons.size(); index++ ) {
        cv::Mat wrapped_plate = WrapPlateContour( source_img, plates_polygons[ index ] );
        wrapped_plate = sourceImageToBinary( wrapped_plate );

        plate_images.push_back( wrapped_plate );
    }

    return plate_images;
}

cv::Mat PlateSegmentation::sourceImageToEdges(cv::Mat *source_image) {
    cv::Mat source_gray;
    cv::cvtColor(*source_image, source_gray, cv::COLOR_BGR2GRAY);

    cv::blur(source_gray, source_gray, cv::Size(3, 3));

    cv::Mat source_edges;
    cv::Canny(source_gray, source_edges, 100, 300, 3, true);

    return source_edges;
}

vector< vector< cv::Point > > PlateSegmentation::getPolygons( cv::Mat & source_edges ) {
    vector< vector< cv::Point > > contours;
    cv::findContours( source_edges, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE );

    vector< vector< cv::Point > > polygons;

    for ( size_t index = 0; index < contours.size(); index++ ) {
        vector< cv::Point > polygon = getPolygonFromContour( contours[ index ] );

        if ( isValidPolygon( polygon ) ) {
            polygon = OrderPolygonCorners( polygon );
            polygons.push_back( polygon );
        }
    }

    return polygons;
}

vector< cv::Point > PlateSegmentation::getPolygonFromContour( vector< cv::Point > contour ) {
    cv::Mat mat_contour( contour );

    vector< cv::Point > hull;
    cv::convexHull( mat_contour, hull );

    double epsilon = 19;
    bool closed = true;

    vector< cv::Point > polygon;
    cv::approxPolyDP( hull, polygon, epsilon, closed );

    return polygon;
}

bool PlateSegmentation::isValidPolygon( vector< cv::Point > polygon ) {
    return polygon.size() == 4;
}

/*
 * Polygons must be ordered starting from lower right corner clockwise
 */
vector< cv::Point > PlateSegmentation::OrderPolygonCorners( vector< cv::Point > polygon ) {
    int total_points = polygon.size();
    int index_max = -1;
    double distance_max = -1;

    for ( int index = 0; index < total_points; index++ ) {
        double point_distance = PointOriginDistance(polygon.at(index));

        if (point_distance > distance_max) {
            index_max = index;
            distance_max = point_distance;
        }
    }

    vector< cv::Point > ordered_polygon;

    for ( int index = index_max; index < total_points; index++ ) {
        ordered_polygon.push_back( polygon.at( index ) );
    }

    for ( int index = 0; index < index_max; index++ ) {
        ordered_polygon.push_back( polygon.at( index ) );
    }

    return ordered_polygon;
}

double PlateSegmentation::PointOriginDistance( cv::Point point ) {
    return sqrt( ( pow( point.x, 2 ) + pow( point.y, 2 ) ) );
}

cv::Mat PlateSegmentation::WrapPlateContour( cv::Mat *source_img, vector< cv::Point > plate_polygons ) {
    cv::Mat warped_plate( PLATE_HEIGHT, PLATE_WIDTH, CV_8UC3 );
    vector< cv::Point> real_plate_polygons;
    real_plate_polygons = { cv::Point(PLATE_WIDTH, PLATE_HEIGHT), cv::Point(0, PLATE_HEIGHT), cv::Point(0, 0), cv::Point(PLATE_WIDTH, 0) };
    cv::Mat homography = cv::findHomography( plate_polygons, real_plate_polygons );
    cv::warpPerspective(*source_img, warped_plate, homography, cv::Size(PLATE_WIDTH, PLATE_HEIGHT));

    return warped_plate;
}

cv::Mat PlateSegmentation::sourceImageToBinary(cv::Mat source_image) {
    cv::Mat gray_source_image;
    cv::cvtColor(source_image, gray_source_image, cv::COLOR_BGR2GRAY);

    cv::Mat binary_img;
    cv::adaptiveThreshold(gray_source_image, binary_img, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 55, 5);

    return binary_img;
}