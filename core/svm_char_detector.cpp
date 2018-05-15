#include "svm_char_detector.hpp"

#include <vector>
#include <string>
#include <dirent.h>

#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml.hpp>

#include "constants.hpp"

using namespace std;


SvmCharDetector::SvmCharDetector() {
    svm_pointer = cv::ml::SVM::create();
}


SvmCharDetector::SvmCharDetector( string trained_data_path ) {
    svm_pointer = cv::ml::SVM::load( trained_data_path );
}


char SvmCharDetector::detectCharFromImage( cv::Mat char_image ) {
    vector<float> image_features = calculateImageFeatures(char_image);

    cv::Mat m = cv::Mat(1, SVM_FEATURES_AMOUNT, CV_32FC1);

    for (size_t i = 0; i < image_features.size(); ++i) {
        float temp = image_features[i];
        m.at<float>(0, i) = temp;
    }

    int ri = int(svm_pointer -> predict( m ));

    if (ri >= 0 && ri <= 9)
        return (char) (ri + '0'); //ma ascii 0 = 48

    if (ri >= 10 && ri < 18)
        return (char) (ri + 55); //ma accii A = 5, --> tu A-H

    if (ri >= 18 && ri < 22)
        return (char) (ri + 55 + 2); //K-N, bo I,J

    if (ri == 22) return 'P';

    if (ri == 23) return 'S';

    if (ri >= 24 && ri < 27)
        return (char) (ri + 60); //T-V,

    if (ri >= 27 && ri < 30)
        return (char) (ri + 61); //X-Z

    return '*';
}


vector<float> SvmCharDetector::calculateImageFeatures(cv::Mat source_image) {
    cv::Mat clean_image = cleanSourceImage( source_image );

    vector<float> image_features;

    cv::resize(clean_image, clean_image, cv::Size(40, 40));
    int img_height = clean_image.rows / 4;
    int img_width = clean_image.cols / 4;
    int S = colorPixelsAmount(clean_image, true);
    int T = clean_image.cols * clean_image.rows;
    for (int i = 0; i < clean_image.rows; i += img_height) {
        for (int j = 0; j < clean_image.cols; j += img_width) {
            cv::Mat cell = clean_image(cv::Rect(i, j, img_height, img_width));
            int s = colorPixelsAmount(cell, true);
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


cv::Mat SvmCharDetector::cleanSourceImage( cv::Mat source_image ) {
    cv::Mat clean_image;
    if (source_image.channels() == 3) {
        cv::cvtColor(source_image, clean_image, cv::COLOR_BGR2GRAY);
        cv::threshold(clean_image, clean_image, 100, 255, cv::THRESH_BINARY);
    } else {
        cv::threshold(source_image, clean_image, 100, 255, cv::THRESH_BINARY);
    }
    return clean_image;
}


int SvmCharDetector::colorPixelsAmount(cv::Mat img, bool black_pixel) {
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


vector<string> SvmCharDetector::getClassesFoldersPaths(string training_set_path) {
    vector<string> folders;
    DIR *dir = opendir(training_set_path.c_str());
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0)) {
            string folder_path = training_set_path + string(entry->d_name);
            folders.push_back(folder_path);
        }
    }
    closedir(dir);

    return folders;
}


vector<string> SvmCharDetector::getClassSamplePath(string class_folder_path) {
    vector<string> files;
    DIR *dir = opendir(class_folder_path.c_str());
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0)) {
            string file_path = class_folder_path + "/" + string(entry->d_name);
            files.push_back(file_path);
        }
    }
    closedir(dir);
    return files;
}


bool SvmCharDetector::train(string training_set_path, string trained_svm_path) {
    if (CLASS_SAMPLES_AMOUNT <= 0 || CLASSES_AMOUNT <= 0) return false;

    svm_pointer -> setType(cv::ml::SVM::C_SVC);
    svm_pointer -> setKernel(cv::ml::SVM::LINEAR);
    svm_pointer -> setGamma(0.5);
    svm_pointer -> setC(16);
    svm_pointer -> setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6));

    vector<string> trainingFoldersPaths = getClassesFoldersPaths(training_set_path);
    int total_class_folders = (int)trainingFoldersPaths.size();

    if (total_class_folders <= 0) {
        cout << "Missing training classes folders under " << training_set_path << endl;
        return false;
    }
    if (total_class_folders != CLASSES_AMOUNT) {
        cout << "Training class folders under " << training_set_path << " should be " << CLASSES_AMOUNT << endl;
        return false;
    }

    cv::Mat samples_matrix = cv::Mat(CLASSES_AMOUNT * CLASS_SAMPLES_AMOUNT, SVM_FEATURES_AMOUNT, CV_32FC1);
    cv::Mat responses_matrix = cv::Mat(CLASSES_AMOUNT * CLASS_SAMPLES_AMOUNT, 1, CV_32SC1);

    int sample_index = 0;
    std::sort(trainingFoldersPaths.begin(), trainingFoldersPaths.end());
    for (size_t folder_index = 0; folder_index < total_class_folders; ++folder_index) {

        string class_folder_path = trainingFoldersPaths.at(folder_index);
        vector<string> class_samples = getClassSamplePath(class_folder_path);
        int total_class_samples = (int)class_samples.size();

        if (total_class_samples <= 0) {
            cout << "Missing class samples under" << class_folder_path << endl;
            return false;
        }
        if (total_class_samples != CLASS_SAMPLES_AMOUNT) {
            cout << "Missing training class samples under" << class_folder_path << "should be" << CLASS_SAMPLES_AMOUNT << endl;
            return false;
        }

        cout << "list folder" << class_folder_path << endl;
        string label_folder = class_folder_path.substr(class_folder_path.length() - 1);

        for (size_t j = 0; j < total_class_samples; ++j) {
            cv::Mat class_sample_image = cv::imread(class_samples.at(j));

            if (class_sample_image.empty()) {
                return false;
            }

            vector<float> class_sample_features = calculateImageFeatures(class_sample_image);

            for (size_t t = 0; t < class_sample_features.size(); ++t)
                samples_matrix.at<float>(sample_index, t) = class_sample_features.at(t);
            responses_matrix.at<int>(sample_index, 0) = folder_index;
            sample_index++;
        }
    }

    svm_pointer -> trainAuto(cv::ml::TrainData::create(samples_matrix, cv::ml::ROW_SAMPLE, responses_matrix));
    svm_pointer -> save(trained_svm_path);

    return true;
}