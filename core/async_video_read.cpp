#include "async_video_read.h"

#include <iostream>
#include <thread>
#include <unistd.h>

#include <opencv2/highgui/highgui.hpp>

AsyncVideoReader::AsyncVideoReader(string stream_name) {
    this -> stream_name = stream_name;
    this -> is_stream_opened = false;
    this -> has_frame = false;

    openStream();

    thread asyncFrameReader(&AsyncVideoReader::readFrame, this);
    asyncFrameReader.detach();
}

void AsyncVideoReader::openStream() {
    cout << "Opening camera stream..." << endl;

    while ( !is_stream_opened ) {
        video_stream = new cv::VideoCapture( stream_name );
        is_stream_opened = video_stream -> isOpened();

        if ( !is_stream_opened ) {
            cout << "Camera not found, is connected to LAN? Retrying..." << endl << endl;
            usleep( 1000000 );
        }
    }
    cout << "Camera found!" << endl;
}


void AsyncVideoReader::readFrame() {
    while ( true ) {
        this -> has_frame = video_stream -> read( current_frame );
    }
}


cv::Mat AsyncVideoReader::getFrame() {
    return current_frame;
}

bool AsyncVideoReader::isOpened() {
    return is_stream_opened;
}

bool AsyncVideoReader::hasFrame() {
    return has_frame;
}
