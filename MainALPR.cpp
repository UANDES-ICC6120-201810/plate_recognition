#include <iostream>
#include <string>
#include <unistd.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "core/PlateFinder.hpp"
#include "core/CharacterFinder.hpp"

using namespace std;

int frame_file_name_counter = 0;

void getPlateFor(string source_name) {
    cv::Mat source_image = cv::imread(source_name);
    vector< cv::Mat > plates = PlateFinder().findPlateImages(source_image);

    if (plates.size() > 0) {
        vector< string > plate_texts = CharacterFinder().findPlatesText(plates);

        for (size_t index = 0; index < plate_texts.size(); index++) {
            cout << plate_texts.at(index) << endl;
        }
    }
}

void getPlateForStream(string stream_name) {

    cv::VideoCapture *stream = new cv::VideoCapture(stream_name);
    bool streamClosed = !stream->isOpened();

    if (streamClosed) return -1;
    cv::Mat source_image;

    while (true) {
        bool noImage = !stream->read(source_image);

        if (noImage) return -1;
        getPlateFor(source_image);
    }
}

int main(int argc, char *argv[]) {

    if ( CV_MAJOR_VERSION < 3 ) {
        cout << "OpenCV version: " << CV_MAJOR_VERSION << endl;
        cout << "Old OpenCV version found, required 3+" << endl;
        return 1;
    }

    string source_file_name;

    if (argc < 2) {
        source_file_name = "rtsp://taller:taller2018@192.168.2.2:88/videoMain";
        getPlateForStream(source_file_name);
    } else if (argc == 2) {
        source_file_name = argv[1];
        getPlateFor(source_file_name);
    } else {
        return 1;
    }


    return 0;
}