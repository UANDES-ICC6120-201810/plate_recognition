#include <iostream>

#include "core/constants.hpp"
#include "core/svm_char_detector.hpp"


using namespace std;


int main( int argc, char *argv[] ) {

    if ( CV_MAJOR_VERSION < 3 ) {
        cout << "Old OpenCV version found, required 3+" << endl;
        return 0;
    }

    SvmCharDetector svm_char_detector;
    bool train = svm_char_detector.train( SVM_TRAINING_DATA_PATH, SVM_TRAINED_DATA_PATH );

    if ( train )
        cout << "SVM Training Completed" << endl;
    else
        cout << " Train ERROR " << endl;

    return 0;
}
