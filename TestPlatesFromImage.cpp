#include <vector>

#include <opencv2/highgui/highgui.hpp>

#include "core/plate_segmentation.hpp"
#include "core/debugger.hpp"

int main( int argc, char *argv[] ) {

    cv::Mat source_img = cv::imread(argv[1]);

    vector< cv::Mat > plates = PlateSegmentation().findPlateImages( source_img );

    debugWriteRecognizedPlates(plates);

    return 0;
}
