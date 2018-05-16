# ALPR script

Automatic license plate recognition uses a trained SVM to detect the chars in
a picture of a car.

## Font for making the dataset is named

> f schrift font

## To compile SVMTrain.cpp
> $ g++ TrainSVM.cpp core/svm_char_detector.cpp $(pkg-config --libs opencv) -o TrainSVM

Then simply run
$ ./SVMTrain

## In case unlinked library consider this
https://stackoverflow.com/questions/12335848/opencv-program-compile-error-libopencv-core-so-2-4-cannot-open-shared-object-f

Run command
> $ echo '/usr/local/lib/' > /etc/ld.so.conf.d/opencv.conf && ldconfig -v


## To compile RecogSVM.cpp
> $ g++ MainALPR.cpp core/plate_segmentation.cpp core/char_segmentation.cpp core/ocr_detector.cpp core/svm_char_detector.cpp core/constants.hpp core/debugger.cpp $(pkg-config --libs opencv) -o MainALPR

Then simply run
> $ ./RecogSVM ./samples/<image_name>


## To copy files into the container
> docker cp <path_to_local_file> <container_id>:<path_to_destination>

# Testing
For testing **Plates** run

> $ g++ TestPlatesFromImage.cpp core/plate_segmentation.cpp core/debugger.cpp $(pkg-config --libs opencv) -o test-plates

For testing **Chars** run

> $ g++ TestCharsFromPlate.cpp core/char_segmentation.cpp core/debugger.cpp $(pkg-config --libs opencv) -o test-chars

NOTE: Use binary images when testing plates


http://www.biobiochile.cl/noticias/2014/01/31/nuevas-placas-patentes-tienen-tipografia-renovada-y-mayores-medidas-de-seguridad.shtml
