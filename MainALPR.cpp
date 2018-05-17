#include <iostream>
#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "core/async_video_read.h"
#include "core/constants.hpp"
#include "core/plate_segmentation.hpp"
#include "core/char_segmentation.hpp"
#include "core/ocr_detector.hpp"

#include "core/debugger.hpp"

using namespace std;

int frame_file_name_counter = 0;

void getPlateFor( cv::Mat source_image ) {
    OcrDetector char_detector( SVM_TRAINED_DATA_PATH );

    vector< cv::Mat > plates = PlateSegmentation().findPlateImages( source_image );


    if ( plates.size() < 1 ) return;

    CharSegmentation char_divider;

    for ( size_t index = 0; index < plates.size(); index++ ) {
        cv::Mat plate_image = plates.at(index);

        vector< cv::Mat > plate_chars = char_divider.findPlateCharImages( plate_image );

        string plate_text = char_detector.plateCharsToString( plate_chars );

        if ( plate_text != EMPTY_PLATE ) cout << plate_text << endl;
    }

}

void getPlateForStream( string stream_name ) {
    AsyncVideoReader *asyncVideoReader = new AsyncVideoReader( stream_name );

    while ( true ) {
        bool no_image = !asyncVideoReader -> hasFrame();
        if ( no_image ) continue;

        cv::Mat source_image = asyncVideoReader -> getFrame();
        getPlateFor( source_image );
    }
}

int main( int argc, char *argv[] ) {

    if ( CV_MAJOR_VERSION < 3 ) {
        cout << "ALPR: OpenCV version: " << CV_MAJOR_VERSION << endl;
        cout << "ALPR: Old OpenCV version found, required 3+" << endl;
        return 1;
    }

    string source_file_name;

    if ( argc < 2 ) {
        source_file_name = "rtsp://taller:taller2018@192.168.1.10:88/videoMain";
        getPlateForStream( source_file_name );

        cout << "ALPR: Ending stream capture..." << endl;

    } else if ( argc == 2 ) {
        source_file_name = argv[1];

        cv::Mat source_image = cv::imread( source_file_name );
        getPlateFor( source_image );

    } else {
        return 1;
    }

    return 0;
}