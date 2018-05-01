#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/ml.hpp"
#include "dirent.h"
#include "feature.h"
#include <iostream>

using namespace cv;
using namespace std;
using namespace cv::ml;
Mat srcImg;

const int CLASSES_AMOUNT = 30;
const int CLASS_SAMPLES_AMOUNT = 10;
const int FEATURES_AMOUNT = 32;

vector<string> getClassesFoldersPaths(string training_set_path) {
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

vector<string> getClassSamplePath(string class_folder_path) {
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

bool TrainSVM(string training_set_path, string trained_svm_path) {
    if (CLASS_SAMPLES_AMOUNT <= 0 || CLASSES_AMOUNT <= 0) return false;

    //Train SVM OpenCV 3.1+
    Ptr <SVM> svm = SVM::create();
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::LINEAR);
    svm->setGamma(0.5);
    svm->setC(16);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));

    vector<string> trainingFoldersPaths = getClassesFoldersPaths(training_set_path);
    int total_class_folders = (int)trainingFoldersPaths.size();

    if (total_class_folders <= 0) {
        cout << "Missing training classes folders under" << training_set_path << endl;
        return false;
    }
    if (total_class_folders != CLASSES_AMOUNT) {
        cout << "Missing training class folders under" << training_set_path << "should be" << CLASSES_AMOUNT << endl;
        return false;
    }

    Mat samples_matrix = Mat(CLASSES_AMOUNT * CLASS_SAMPLES_AMOUNT, FEATURES_AMOUNT, CV_32FC1);
    Mat responses_matrix = Mat(CLASSES_AMOUNT * CLASS_SAMPLES_AMOUNT, 1, CV_32SC1);

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
            Mat class_sample_image = imread(class_samples.at(j));

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

    // SVM Train OpenCV 3.1+
    svm->trainAuto(ml::TrainData::create(samples_matrix, ml::ROW_SAMPLE, responses_matrix));
    svm->save(trained_svm_path);
    return true;
}

int main(int argc, char *argv[]) {

    if ( CV_MAJOR_VERSION < 3 ) {
        cout << "Old OpenCV version found, required 3+" << endl;
        return 0;
    }

    std::string training_set_path = "./data/";
    std::string trained_svm_path = "svm.txt";

    bool train = TrainSVM(training_set_path, trained_svm_path);
    if (train) {
        cout << "SVM Training Completed" << endl;
    } else
        cout << " Train ERROR " << endl;
    cout << "Press any key to exit." << endl;
    getwchar();
    return 0;

}
