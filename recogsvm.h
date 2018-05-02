#ifndef ALPR_RECOGSVM_H
#define ALPR_RECOGSVM_H

    char detectCharFromImage(Mat char_image);
    bool validPlateDimensions(Rect plate_rect);
    vector< vector<cv::Point> > getPotentialPlatesCharsContours(Mat plate_image);
    bool validPlateCharDimensions(Rect plate_rect, Rect sub_r);
    bool validPixelRatio(double pixel_ratio);
    bool imageShouldBeChar(Mat plate_char_image);
    vector<Rect> getDetectedPlateChars(Rect plate_rect, Mat plate_image, vector<vector<cv::Point> > potential_plate_chars_contours);
    vector<Rect> sortPlateCharsByX(vector<Rect> detected_plate_chars);
    vector<Mat> getCharImages(Mat plate_image, vector<Rect> detected_plate_chars);
    vector< vector<Mat> > getDetectedPlates(Mat original_binary_src_img, vector< vector<cv::Point> > potential_plates_contours);
    string plateCharsToString(vector<Mat> detected_plate);
    Mat sourceImageToBinary(Mat source_image);
    vector< vector<cv::Point> > getPotentialPlatesContours(Mat original_binary_src_img);
    string SVMPredict(Mat source_image);

#endif //ALPR_RECOGSVM_H
