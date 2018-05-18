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

    cv::Mat features = cv::Mat(image_features).t();

    int folder_label = int(svm_pointer -> predict( features ));

    if (0 <= folder_label && folder_label <= 9)
        return (char) (folder_label + '0');

    if (A_FOLDER <= folder_label && folder_label <= H_FOLDER)
        return (char) (folder_label + 55);

    int skip_i_j = 2;

    if (K_FOLDER <= folder_label && folder_label <= N_FOLDER)
        return (char) (folder_label + 55 + skip_i_j);

    if (folder_label == P_FOLDER) return 'P';

    if (folder_label == S_FOLDER) return 'S';

    if (T_FOLDER <= folder_label && folder_label <= V_FOLDER)
        return (char) (folder_label + 60);

    if (X_FOLDER <= folder_label && folder_label <= Z_FOLDER)
        return (char) (folder_label + 61);

    return '*';
}


vector<float> SvmCharDetector::calculateImageFeatures( cv::Mat source_image ) {
    cv::Mat clean_image = cleanSourceImage( source_image );

    vector< float > image_features;

    int feature_index = 0;

    int image_color_pixels = colorPixelsAmount( clean_image, true );

    for ( int row_index = 0; row_index < clean_image.rows; row_index += CELL_HEIGHT ) {
        for ( int col_index = 0; col_index < clean_image.cols; col_index += CELL_WIDTH ) {

            cv::Mat cell = clean_image( cv::Rect( row_index, col_index, CELL_HEIGHT, CELL_WIDTH ) );

            int cell_color_pixels = colorPixelsAmount( cell, true );
            float feature = ( float ) cell_color_pixels / image_color_pixels;

            image_features.push_back( feature );
        }
    }

    for ( int i = 0; i < 16; i += 4 ) {
        float feature = image_features[ i ];
        feature += image_features[ i + 1 ];
        feature += image_features[ i + 2 ];
        feature += image_features[ i + 3 ];

        image_features.push_back( feature );
    }

    for ( int i = 0; i < 4; i++ ) {
        float feature = image_features[ i ];
        feature += image_features[ i + 4 ];
        feature += image_features[ i + 8 ];
        feature += image_features[ i + 12 ];

        image_features.push_back( feature );
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
    cv::Mat clean_image = source_image.clone();

    if (source_image.channels() == 3)
        cv::cvtColor(clean_image, clean_image, cv::COLOR_BGR2GRAY);

    cv::threshold(clean_image, clean_image, 100, 255, cv::THRESH_BINARY);

    cv::resize( clean_image, clean_image, cv::Size( FEATURE_WIDTH, FEATURE_HEIGHT ) );

    return clean_image;
}


int SvmCharDetector::colorPixelsAmount(cv::Mat img, bool black_pixel) {
    int pixels = 0;

    for ( int row_index = 0; row_index < img.rows; row_index++ ) {
        cv::Mat row_img = img( cv::Rect( 0, row_index, img.cols, 1 ) );

        int row_white_pixels = cv::countNonZero( row_img );

        if ( black_pixel )
            pixels += img.cols - row_white_pixels;
        else
            pixels += row_white_pixels;
    }

    return pixels;
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
    for ( int folder_index = 0; folder_index < total_class_folders; folder_index++) {

        string class_folder_path = trainingFoldersPaths.at(folder_index);
        vector<string> class_samples_paths = getClassSamplePath(class_folder_path);
        int total_class_samples = (int)class_samples_paths.size();

        if (total_class_samples <= 0) {
            cout << "Missing class samples under" << class_folder_path << endl;
            return false;
        }
        if (total_class_samples != CLASS_SAMPLES_AMOUNT) {
            cout << "Training class samples under " << class_folder_path << " should be " << CLASS_SAMPLES_AMOUNT << endl;
            return false;
        }

        cout << "list folder " << class_folder_path << endl;
        string label_folder = class_folder_path.substr(class_folder_path.length() - 1);

        for ( int j = 0; j < CLASS_SAMPLES_AMOUNT; j++ ) {
            cv::Mat class_sample_image = cv::imread(class_samples_paths.at(j));

            if (class_sample_image.empty()) {
                return false;
            }

            vector<float> class_sample_features = calculateImageFeatures( class_sample_image );

            for ( int t = 0; t < SVM_FEATURES_AMOUNT; t++ )
                samples_matrix.at<float>(sample_index, t) = class_sample_features.at(t);

            responses_matrix.at<int>(sample_index, 0) = folder_index;
            sample_index++;
        }
    }

    svm_pointer -> trainAuto(cv::ml::TrainData::create(samples_matrix, cv::ml::ROW_SAMPLE, responses_matrix));
    svm_pointer -> save(trained_svm_path);

    return true;
}