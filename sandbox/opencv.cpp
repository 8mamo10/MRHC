#include "opencv2/opencv.hpp"

using namespace cv;

int main(int argc, char** argv)
{
    if (argc != 3) {
        printf("usage: DisplayImage.out <from_path> <to_path> \n");
        return -1;
    }
    Mat image;
    image = imread(argv[1], 1);
    //image = imread(argv[1], 0);
    if (!image.data) {
        printf("No image data \n");
        return -1;
    }
    rectangle(image, Point(10, 10), Point(50, 50), Scalar(255, 255, 255), 1, LINE_AA);
    rectangle(image, Point(60, 60), Point(100, 100), Scalar(255, 0, 0), 10, LINE_8);
    rectangle(image, Point(110, 110), Point(150, 150), Scalar(0, 0, 0), -1, LINE_8);
    imwrite(argv[2], image);
    return 0;
}
// g++ opencv.cpp  -std=c++0x `pkg-config --cflags --libs opencv4`
