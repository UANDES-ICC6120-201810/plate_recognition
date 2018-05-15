#include <iostream>

#include "core/svm_char_detector.hpp"


using namespace std;


int main( int argc, char *argv[] ) {

    if ( CV_MAJOR_VERSION < 3 ) {
        cout << "Old OpenCV version found, required 3+" << endl;
        return 0;
    }

    string training_set_path = "./data/";
    string trained_svm_path = "svm.txt";

    SvmCharDetector svm_char_detector;

    bool train = svm_char_detector.train( training_set_path, trained_svm_path );

    if ( train )
        cout << "SVM Training Completed" << endl;
    else
        cout << " Train ERROR " << endl;

    return 0;
}
