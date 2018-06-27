#include <iostream>
#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "core/async_video_read.h"
#include "core/constants.hpp"
#include "core/plate_segmentation.hpp"
#include "core/char_segmentation.hpp"
#include "core/ocr_detector.hpp"
#include "core/mysql_connector.h"

using namespace std;

MysqlConnector *db_conn;
OcrDetector *char_detector;
CharSegmentation *char_divider;
PlateSegmentation *plate_divider;

void getPlateFor( cv::Mat *source_image ) {
    vector< cv::Mat > plates = plate_divider -> findPlateImages( source_image );

    if ( plates.size() < 1 ) return;

    for ( size_t index = 0; index < plates.size(); index++ ) {
        cv::Mat *plate_image = &plates.at(index);

        vector< cv::Mat > plate_chars = char_divider -> findPlateCharImages( plate_image );

        string plate_text = char_detector -> plateCharsToString( plate_chars );

        if ( plate_text != EMPTY_PLATE ) {
            cout << plate_text << endl;
            db_conn -> async_post( plate_text );
        }
    }

}

void getPlateForStream( string stream_name ) {
    AsyncVideoReader *asyncVideoReader = new AsyncVideoReader( stream_name );

    while ( true ) {
        bool no_image = !asyncVideoReader -> hasFrame();
        if ( no_image ) {
            continue;
        }

        cv::Mat source_image = asyncVideoReader -> getFrame();
        getPlateFor( &source_image );
    }
}

int main( int argc, char *argv[] ) {

    if ( CV_MAJOR_VERSION < 3 ) {
        cout << "ALPR: OpenCV version: " << CV_MAJOR_VERSION << endl;
        cout << "ALPR: Old OpenCV version found, required 3+" << endl;
        return 1;
    }

    string source_file_name;

    char_detector = new OcrDetector( SVM_TRAINED_DATA_PATH );
    db_conn = new MysqlConnector(DATABASE_HOST_PORT, DATABASE_USER, DATABASE_USER_PASS, DATABASE_NAME);
    plate_divider = new PlateSegmentation();

    if ( argc < 2 ) {
        source_file_name = CAMERA_STREAM_URL;
        getPlateForStream( source_file_name );

        cout << "ALPR: Ending stream capture..." << endl;

    } else if ( argc == 2 ) {
        source_file_name = argv[1];

        cv::Mat source_image = cv::imread( source_file_name );
        getPlateFor( &source_image );

    } else {
        return 1;
    }

    return 0;
}
