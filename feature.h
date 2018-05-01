#ifndef FEATURE_H
#define FEATURE_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include "dirent.h"

using namespace std;
using namespace cv;

const int number_of_feature = 32;

static int count_pixel(Mat img, bool black_pixel = true) {
    int black = 0;
    int white = 0;

    for (int row = 0; row < img.rows; ++row)
        for (int col = 0; col < img.cols; ++col) {
            if (img.at<uchar>(row, col) == 0)
                black++;
            else
                white++;
        }
    if (black_pixel)
        return black;
    else
        return white;
}

static vector<float> calculateImageFeatures(Mat src_image) {
    Mat img;
    if (src_image.channels() == 3) {
        cvtColor(src_image, img, cv::COLOR_BGR2GRAY);
        threshold(img, img, 100, 255, cv::THRESH_BINARY);
    } else {
        threshold(src_image, img, 100, 255, cv::THRESH_BINARY);
    }

    vector<float> image_features;

    resize(img, img, Size(40, 40));
    int h = img.rows / 4;
    int w = img.cols / 4;
    int S = count_pixel(img);
    int T = img.cols * img.rows;
    for (int i = 0; i < img.rows; i += h) {
        for (int j = 0; j < img.cols; j += w) {
            Mat cell = img(Rect(i, j, h, w));
            int s = count_pixel(cell);
            float f = (float) s / S;
            image_features.push_back(f);
        }
    }

    for (int i = 0; i < 16; i += 4) {
        float f = image_features[i] + image_features[i + 1] + image_features[i + 2] + image_features[i + 3];
        image_features.push_back(f);
    }

    for (int i = 0; i < 4; ++i) {
        float f = image_features[i] + image_features[i + 4] + image_features[i + 8] + image_features[i + 12];
        image_features.push_back(f);
    }

    image_features.push_back(image_features[0] + image_features[5] + image_features[10] + image_features[15]);
    image_features.push_back(image_features[3] + image_features[6] + image_features[9] + image_features[12]);
    image_features.push_back(image_features[0] + image_features[1] + image_features[4] + image_features[5]);
    image_features.push_back(image_features[2] + image_features[3] + image_features[6] + image_features[7]);
    image_features.push_back(image_features[8] + image_features[9] + image_features[12] + image_features[13]);
    image_features.push_back(image_features[10] + image_features[11] + image_features[14] + image_features[15]);
    image_features.push_back(image_features[5] + image_features[6] + image_features[9] + image_features[10]);
    image_features.push_back(image_features[0] + image_features[1] + image_features[2] + image_features[3] + image_features[4] + image_features[7] + image_features[8] + image_features[11] + image_features[12] + image_features[13] + image_features[14] + image_features[15]);

    return image_features; //32 feature
}


#endif
