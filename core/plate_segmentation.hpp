#ifndef PLATE_SEGMENTATION_H
#define PLATE_SEGMENTATION_H

#include <vector>
#include "opencv2/core/mat.hpp"

using namespace std;

class PlateSegmentation {
public:
    vector< cv::Mat > findPlateImages( cv::Mat source_img );

private:
    cv::Mat sourceImageToEdges(cv::Mat source_image);
    vector< vector< cv::Point > > getPolygons( cv::Mat & source_edges );

    vector< cv::Point > getPolygonFromContour( vector< cv::Point > contour );
    bool isValidPolygon( vector< cv::Point > polygon );

    vector< cv::Point > OrderPolygonCorners( vector< cv::Point > polygon );
    double PointOriginDistance( cv::Point point );

    cv::Mat WrapPlateContour( cv::Mat source_img, vector< cv::Point > plate_polygons );
    cv::Mat sourceImageToBinary(cv::Mat source_image);
};

#endif
