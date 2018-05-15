#ifndef SVM_CHAR_DETECTOR_H
#define SVM_CHAR_DETECTOR_H

#include <vector>
#include <string>
#include <opencv2/core/mat.hpp>
#include <opencv2/ml.hpp>

using namespace std;

class SvmCharDetector {
public:
    SvmCharDetector( string trained_data_path );
    SvmCharDetector();

    bool train(string training_set_path, string trained_svm_path);
    char detectCharFromImage( cv::Mat char_image );

private:
    cv::Ptr< cv::ml::SVM > svm_pointer;

    vector<float> calculateImageFeatures( cv::Mat source_image );
    cv::Mat cleanSourceImage( cv::Mat source_image );
    int colorPixelsAmount(cv::Mat img, bool black_pixel = true);

    vector<string> getClassesFoldersPaths(string training_set_path);
    vector<string> getClassSamplePath(string class_folder_path);
};

#endif
