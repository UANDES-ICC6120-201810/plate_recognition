# ALPR script

Automatic license plate recognition uses a trained SVM to detect the chars in
a picture of a car.

## Font for making the dataset is named

> f schrift font

## To compile SVMTrain.cpp
> $ g++ SVMTrain.cpp $(pkg-config --libs opencv) -o SVMTrain

Then simply run
$ ./SVMTrain

## In case unlinked library consider this
https://stackoverflow.com/questions/12335848/opencv-program-compile-error-libopencv-core-so-2-4-cannot-open-shared-object-f

Create file
> $ echo '/usr/local/lib/' > /etc/ld.so.conf.d/opencv.conf

Then run
> $ ldconfig -v


## To compile RecogSVM.cpp
> $ g++ RecogSVM.cpp $(pkg-config --cflags --libs opencv) -o RecogSVM

Then simply run
> $ ./RecogSVM "./samples/<image_name>"


## To copy files into the container
> docker cp <path_to_local_file> <container_id>:<path_to_destination>
