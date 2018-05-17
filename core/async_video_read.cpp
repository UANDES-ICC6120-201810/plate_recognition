#include "async_video_read.h"

#include <thread>
#include <opencv2/highgui/highgui.hpp>

AsyncVideoReader::AsyncVideoReader(string stream_name) {
    video_stream = new cv::VideoCapture( stream_name );
    is_stream_opened = false;
    has_frame = false;

    thread asyncFrameReader(&AsyncVideoReader::readFrame, this);
    asyncFrameReader.detach();
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