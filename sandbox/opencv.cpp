#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv )
{
    if (argc != 3) {
        printf("usage: DisplayImage.out <from_path> <to_path> \n");
        return -1;
    }
    Mat image;
    //image = imread(argv[1], 1);
    image = imread(argv[1], 0);
    if (!image.data) {
        printf("No image data \n");
        return -1;
    }
    rectangle(image, Point(10, 10), Point(100, 100), Scalar(255, 255, 255), 5, CV_AA);
    rectangle(image, Point(110, 110), Point(120, 120), Scalar(0, 0, 0), -1, CV_AA);
    imwrite(argv[2], image);
    return 0;
}

// g++ opencv.cpp `pkg-config --cflags opencv` `pkg-config --libs opencv` 
