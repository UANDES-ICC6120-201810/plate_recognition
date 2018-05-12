#include <vector>

#include <opencv2/highgui/highgui.hpp>

#include "core/char_segmentation.hpp"
#include "core/debugger.hpp"

int main(int argc, char *argv[]) {

    cv::Mat source_img = cv::imread(argv[1]);

    vector< cv::Mat > plate_chars = CharSegmentation().findPlateCharImages( source_img );

    debugWriteRecognizedChars(plate_chars);

    return 0;
}
