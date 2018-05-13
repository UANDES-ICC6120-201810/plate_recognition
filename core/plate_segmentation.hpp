#ifndef PLATE_SEGMENTATION_H
#define PLATE_SEGMENTATION_H

#include <vector>
#include "opencv2/core/mat.hpp"

using namespace std;

class PlateSegmentation {
    public:
        vector< cv::Mat > findPlateImages(cv::Mat source_img);
        vector< cv::Mat > findPlateImagesH(cv::Mat source_img);

    private:
        vector< cv::Rect > findPotentialPlateContours(cv::Mat source_img);
        cv::Mat sourceImageToBinary(cv::Mat source_image);
        vector< cv::Rect > transformContoursToRects(vector< vector< cv::Point > > plate_contours);
        vector< cv::Rect > removeInvalidPlateContours(vector< cv::Rect > plate_contours);
        bool validPlateDimensions(cv::Rect plate_rect);
        vector< cv::Mat > getPlateImages(cv::Mat source_img, vector< cv::Rect > plate_contours);
        cv::Mat getPlateImage(cv::Mat original_img, cv::Rect plate_contour);

        cv::Mat sourceImageToEdges(cv::Mat source_image);
        vector< vector< cv::Point > > getContours( cv::Mat & source_edges );
        cv::Mat WrapPlateContour( cv::Mat source_img, vector< cv::Point > plate_polygons );

        vector< cv::Point > OrderPolygonCorners( vector< cv::Point > polygon );
        double PointOriginDistance( cv::Point point );
};

#endif
