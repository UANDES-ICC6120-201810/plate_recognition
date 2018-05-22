#ifndef ASYNC_VIDEO_READ_H
#define ASYNC_VIDEO_READ_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/mat.hpp>

using namespace std;

class AsyncVideoReader {
public:
    AsyncVideoReader(string stream_name);
    void readFrame();
    cv::Mat getFrame();
    bool isOpened();
    bool hasFrame();
    void endThread();

private:
    cv::VideoCapture *video_stream;
    cv::Mat current_frame;
    string stream_name;
    bool is_stream_opened;
    bool has_frame;
    bool thread_alive;

    void openStream();
};

#endif
