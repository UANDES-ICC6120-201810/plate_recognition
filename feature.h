#ifndef FEATURE_H
#define FEATURE_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include "dirent.h"

using namespace std;
using namespace cv;

const int number_of_feature = 32;

static int colorPixelsAmount(Mat img, bool black_pixel = true) {
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

Mat cleanSourceImage(Mat source_image) {
    Mat clean_image;
    if (src_image.channels() == 3) {
        cvtColor(src_image, clean_image, cv::COLOR_BGR2GRAY);
        threshold(clean_image, clean_image, 100, 255, cv::THRESH_BINARY);
    } else {
        threshold(src_image, clean_image, 100, 255, cv::THRESH_BINARY);
    }
    return clean_image;
}

static vector<float> calculateImageFeatures(Mat source_image) {
    Mat clean_image = cleanSourceImage(source_image);

    vector<float> image_features;

    resize(clean_image, clean_image, Size(40, 40));
    int img_height = clean_image.rows / 4;
    int img_width = clean_image.cols / 4;
    int S = colorPixelsAmount(clean_image);
    int T = clean_image.cols * clean_image.rows;
    for (int i = 0; i < clean_image.rows; i += img_height) {
        for (int j = 0; j < clean_image.cols; j += img_width) {
            Mat cell = clean_image(Rect(i, j, img_height, img_width));
            int s = colorPixelsAmount(cell);
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
