# ALPR script

Automatic license plate recognition uses a trained SVM to detect the chars in
a picture of a car.

## Font for making the dataset is named

> f schrift font

## To compile SVMTrain.cpp
> $ g++ TrainSVM.cpp $(pkg-config --libs opencv) -o TrainSVM

Then simply run
$ ./SVMTrain

## In case unlinked library consider this
https://stackoverflow.com/questions/12335848/opencv-program-compile-error-libopencv-core-so-2-4-cannot-open-shared-object-f

Run command
> $ echo '/usr/local/lib/' > /etc/ld.so.conf.d/opencv.conf && ldconfig -v


## To compile RecogSVM.cpp
> $ g++ MainALPR.cpp core/PlateFinder.cpp core/CharacterFinder.cpp core/SVMCharDetector.hpp core/Constants.hpp core/debugger.cpp $(pkg-config --libs opencv) -o main.out

Then simply run
> $ ./RecogSVM ./samples/<image_name>


## To copy files into the container
> docker cp <path_to_local_file> <container_id>:<path_to_destination>


http://www.biobiochile.cl/noticias/2014/01/31/nuevas-placas-patentes-tienen-tipografia-renovada-y-mayores-medidas-de-seguridad.shtml
