#ifndef ALPR_SVMCHARDETECTOR_H
#define ALPR_SVMCHARDETECTOR_H

#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml.hpp>

#include "Constants.hpp"

static int colorPixelsAmount2(cv::Mat img, bool black_pixel = true) {
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

cv::Mat cleanSourceImage(cv::Mat source_image) {
    cv::Mat clean_image;
    if (source_image.channels() == 3) {
        cv::cvtColor(source_image, clean_image, cv::COLOR_BGR2GRAY);
        cv::threshold(clean_image, clean_image, 100, 255, cv::THRESH_BINARY);
    } else {
        cv::threshold(source_image, clean_image, 100, 255, cv::THRESH_BINARY);
    }
    return clean_image;
}

static vector<float> calculateImageFeatures(cv::Mat source_image) {
    cv::Mat clean_image = cleanSourceImage(source_image);

    vector<float> image_features;

    cv::resize(clean_image, clean_image, cv::Size(40, 40));
    int img_height = clean_image.rows / 4;
    int img_width = clean_image.cols / 4;
    int S = colorPixelsAmount2(clean_image);
    int T = clean_image.cols * clean_image.rows;
    for (int i = 0; i < clean_image.rows; i += img_height) {
        for (int j = 0; j < clean_image.cols; j += img_width) {
            cv::Mat cell = clean_image(cv::Rect(i, j, img_height, img_width));
            int s = colorPixelsAmount2(cell);
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

char detectCharFromImage(cv::Mat char_image) {
    //Load SVM training file OpenCV 3.1
    cv::Ptr< cv::ml::SVM > svmNew = cv::ml::SVM::create();
    svmNew = cv::ml::SVM::load(SVM_TRAINED_DATA_PATH);


    vector<float> image_features = calculateImageFeatures(char_image);
    // Open CV3.1
    cv::Mat m = cv::Mat(1, SVM_FEATURES_AMOUNT, CV_32FC1);
    for (size_t i = 0; i < image_features.size(); ++i) {
        float temp = image_features[i];
        m.at<float>(0, i) = temp;
    }

    int ri = int(svmNew->predict(m)); // Open CV 3.1

    char c = '*';
    if (ri >= 0 && ri <= 9)
        c = (char) (ri + 48); //ma ascii 0 = 48
    if (ri >= 10 && ri < 18)
        c = (char) (ri + 55); //ma accii A = 5, --> tu A-H
    if (ri >= 18 && ri < 22)
        c = (char) (ri + 55 + 2); //K-N, bo I,J
    if (ri == 22) c = 'P';
    if (ri == 23) c = 'S';
    if (ri >= 24 && ri < 27)
        c = (char) (ri + 60); //T-V,
    if (ri >= 27 && ri < 30)
        c = (char) (ri + 61); //X-Z

    return c;
}

#endif
