#include "async_video_read.h"

#include <iostream>
#include <thread>
#include <unistd.h>
#include <opencv2/highgui/highgui.hpp>

AsyncVideoReader::AsyncVideoReader(string stream_name) {

    this -> stream_name = stream_name;

    openStream();

    has_frame = false;

    thread asyncFrameReader(&AsyncVideoReader::readFrame, this);
    asyncFrameReader.detach();
}

void AsyncVideoReader::openStream() {
    while ( !is_stream_opened ) {
        video_stream = new cv::VideoCapture( stream_name );
        is_stream_opened = video_stream -> isOpened();

        if ( !is_stream_opened ) {
            cout << "Camera not found, is connected to LAN? Retrying in 1 second..." << endl << endl;
            usleep( 1000000 );
        }
    }
    cout << "Camera found!" << endl;
}


void AsyncVideoReader::readFrame() {
    while ( true ) {
        is_stream_opened = video_stream -> isOpened();

        if ( is_stream_opened )
            has_frame = video_stream -> read( current_frame );
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