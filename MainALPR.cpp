#include <iostream>

#include <opencv2/highgui/highgui.hpp>

#include "core/PlateFinder.hpp"
#include "core/CharacterFinder.hpp"

using namespace std;

int main(int argc, char *argv[]) {

    if ( CV_MAJOR_VERSION < 3 ) {
        cout << "Old OpenCV version found, required 3+" << endl;
        return 0;
    }

    if (argc < 2) {
        cout << "Missing File Argument" << endl;
        return 1;
    }

    cv::Mat source_image = cv::imread(argv[1]);
    vector< cv::Mat > plates = PlateFinder().findPlateImages(source_image);

    if (plates.size() > 0) {
        vector< string > plate_texts = CharacterFinder().findPlatesText(plates);

        for (size_t index = 0; index < plate_texts.size(); index++) {
            cout << plate_texts.at(index) << endl;
        }

    } else {
        cout << "No plates found" << endl;
    }

    return 0;
}